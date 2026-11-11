#pragma once
#include <cstdint>
#include <cstdio>
#include <string>

// Deliberately independent of transport and telemetry: FC03 reads only.
namespace EasunDiagnostics {
constexpr uint16_t maxWords = 15;
struct Request { uint16_t address = 0; uint16_t count = 0; };
inline bool space(char c) { return c == ' ' || c == '\t'; }
inline bool number(const char *&p, uint16_t &value) {
    while (space(*p)) ++p;
    unsigned base = 10, n = 0, digits = 0;
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) { base = 16; p += 2; }
    while (*p && !space(*p)) {
        unsigned d;
        if (*p >= '0' && *p <= '9') d = *p - '0';
        else if (*p >= 'a' && *p <= 'f') d = *p - 'a' + 10;
        else if (*p >= 'A' && *p <= 'F') d = *p - 'A' + 10;
        else return false;
        if (d >= base || n > (65535U - d) / base) return false;
        n = n * base + d; ++p; ++digits;
    }
    value = static_cast<uint16_t>(n);
    return digits != 0;
}
inline bool parse(const char *p, Request &out) {
    Request candidate;
    if (!number(p, candidate.address)) return false;
    while (space(*p)) ++p;
    candidate.count = 1;
    if (*p && !number(p, candidate.count)) return false;
    while (space(*p)) ++p;
    if (*p || candidate.count == 0 || candidate.count > maxWords ||
        static_cast<uint32_t>(candidate.address) + candidate.count > 65536U) return false;
    out = candidate;
    return true;
}
template<class Reader>
std::string read(const Request &request, Reader reader) {
    if (!request.count || request.count > maxWords ||
        static_cast<uint32_t>(request.address) + request.count > 65536U)
        return "ERROR: invalid EASUN read range";
    uint16_t words[maxWords] = {};
    uint8_t result = 0xFF;
    char line[120];
    if (!reader(request.address, request.count, words, result)) {
        snprintf(line, sizeof line, "ERROR: EASUN FC03 start=0x%04X count=%u result=0x%02X (timeout/CRC/Modbus exception; no values)",
                 unsigned(request.address), unsigned(request.count), unsigned(result));
        return line;
    }
    std::string output;
    for (uint16_t i = 0; i < request.count; ++i) {
        const int32_t signedValue = words[i] < 0x8000 ? int32_t(words[i]) : int32_t(words[i]) - 65536;
        snprintf(line, sizeof line, "0x%04X (%u): hex=0x%04X u16=%u i16=%ld\n",
                 unsigned(request.address + i), unsigned(request.address + i),
                 unsigned(words[i]), unsigned(words[i]), static_cast<long>(signedValue));
        output += line;
    }
    return output;
}
} // namespace EasunDiagnostics
