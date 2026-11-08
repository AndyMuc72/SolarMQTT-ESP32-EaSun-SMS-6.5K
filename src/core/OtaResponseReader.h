#pragma once
#include <stddef.h>
#include <stdint.h>

// NetworkClientSecure::read() may return -1 between TLS records while the
// socket is still connected. Do not turn this transient gap into JSON EOF.
template <typename Transport, typename Clock>
class OtaResponseReader {
public:
    OtaResponseReader(Transport &transport, Clock &clock, uint32_t timeoutMs)
        : _transport(transport), _clock(clock), _timeoutMs(timeoutMs) {}

    int read() {
        if (_position < _length) { ++_bytesRead; return _buffer[_position++]; }
        const uint32_t started = _clock.now();
        while (true) {
            const int available = _transport.available();
            if (available > 0) {
                const size_t wanted = static_cast<size_t>(available) < sizeof(_buffer)
                    ? static_cast<size_t>(available) : sizeof(_buffer);
                const int received = _transport.read(_buffer, wanted);
                if (received > 0) {
                    _length = static_cast<size_t>(received);
                    _position = 1;
                    ++_bytesRead;
                    return _buffer[0];
                }
            }
            if (!_transport.connected()) return -1;
            if (static_cast<uint32_t>(_clock.now() - started) >= _timeoutMs) {
                _timedOut = true;
                return -1;
            }
            _clock.pause();
        }
    }

    size_t readBytes(char *buffer, size_t length) {
        size_t done = 0;
        while (done < length) {
            const int value = read();
            if (value < 0) break;
            buffer[done++] = static_cast<char>(value);
        }
        return done;
    }
    size_t bytesRead() const { return _bytesRead; }
    bool timedOut() const { return _timedOut; }

private:
    Transport &_transport;
    Clock &_clock;
    uint32_t _timeoutMs;
    uint8_t _buffer[256] = {};
    size_t _position = 0, _length = 0, _bytesRead = 0;
    bool _timedOut = false;
};
