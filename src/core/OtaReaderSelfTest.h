#pragma once
#include <ArduinoJson.h>
#include "core/OtaResponseReader.h"
#include <string.h>

inline bool runOtaReaderSelfTests() {
    struct Clock {
        uint32_t tick = 0;
        uint32_t now() const { return tick; }
        void pause() { ++tick; }
    };
    struct Transport {
        const char *text;
        size_t position = 0;
        uint32_t calls = 0;
        bool gap = true, transientFailure = false, stall = false;
        int available() {
            ++calls;
            if (stall || (gap && calls % 3 != 0)) return 0;
            return text[position] ? 1 : 0;
        }
        int read(uint8_t *buffer, size_t) {
            if (transientFailure) { transientFailure = false; return -1; }
            if (!text[position]) return -1;
            *buffer = static_cast<uint8_t>(text[position++]);
            return 1;
        }
        bool connected() const { return stall || text[position] != '\0'; }
    };
    unsigned passed = 0;
    for (unsigned test = 0; test < 6; ++test) {
        Clock clock;
        Transport transport{"{\"ok\":true}"};
        if (test == 1) transport.transientFailure = true;
        if (test == 2) transport.text = "{\"ok\":";
        if (test == 3) transport.stall = true;
        if (test == 4) clock.tick = UINT32_MAX - 4;
        if (test == 5) transport.text = "";
        OtaResponseReader<Transport, Clock> reader(transport, clock, 12);
        JsonDocument doc;
        const auto error = deserializeJson(doc, reader);
        bool ok = false;
        if (test == 2) ok = error == DeserializationError::IncompleteInput;
        else if (test == 3) ok = error && reader.timedOut() && clock.tick == 12;
        else if (test == 5) ok = error == DeserializationError::EmptyInput && !reader.timedOut();
        else ok = !error && doc["ok"].as<bool>() && reader.bytesRead() == strlen(transport.text) && !reader.timedOut();
        if (ok) ++passed;
        Serial.printf("[OTA SELFTEST] case=%u %s\n", test, ok ? "PASS" : "FAIL");
    }
    Serial.printf("[OTA SELFTEST] %u/6 passed\n", passed);
    return passed == 6;
}
