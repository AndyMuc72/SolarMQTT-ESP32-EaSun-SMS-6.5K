#include "modbus/easun_diagnostics.h"
#include <cassert>
#include <iostream>
using namespace EasunDiagnostics;
int main() {
    Request r;
    assert(parse("0x0152", r) && r.address == 338 && r.count == 1);
    assert(parse("338 15", r) && r.address == 338 && r.count == 15);
    assert(parse("0XFFFF 1", r) && r.address == 65535);
    for (const char *bad : {"", "-1", "+1", "0x", "010a", "65536", "999999999999999999999", "1 0", "1 16", "65535 2", "1 1 extra", "1 0x10000", "1\n2", "1;write 2", "1 -1"})
        assert(!parse(bad, r));
    int calls = 0;
    auto reader = [&calls](uint16_t address, uint16_t count, uint16_t *words, uint8_t &result) {
        ++calls; assert(address == 0x219 && count == 4);
        words[0] = 0; words[1] = 32767; words[2] = 32768; words[3] = 65535; result = 0; return true;
    };
    auto text = read({0x219, 4}, reader);
    assert(calls == 1);
    assert(text.find("0x0219 (537): hex=0x0000 u16=0 i16=0") != std::string::npos);
    assert(text.find("i16=32767") != std::string::npos);
    assert(text.find("u16=32768 i16=-32768") != std::string::npos);
    assert(text.find("u16=65535 i16=-1") != std::string::npos);
    for (uint8_t failure : {uint8_t(2), uint8_t(0xE2), uint8_t(0xE3)}) {
        text = read({1, 1}, [failure](uint16_t, uint16_t, uint16_t *words, uint8_t &result) {
            words[0] = 1234; result = failure; return false;
        });
        assert(text.find("ERROR:") == 0 && text.find("u16=") == std::string::npos);
        assert(text.find("result=0x") != std::string::npos);
    }
    for (Request invalid : {Request{0, 0}, Request{0, 16}, Request{65535, 2}}) {
        assert(read(invalid, reader).find("ERROR:") == 0);
        assert(calls == 1); // rejected ranges must never access transport
    }
    for (unsigned address = 0; address <= 65535; ++address) {
        const auto input = std::to_string(address) + " 15";
        assert(parse(input.c_str(), r) == (address <= 65521));
    }
    // Check actual preset requests, timeout continuation and unavailable raw values.
    for (const std::string preset : {"bms", "thermal", "compare"}) {
        std::string requested;
        unsigned transactions = 0;
        text = snapshot(preset, [&](uint16_t a, uint16_t n, uint16_t *w, uint8_t &result) {
            assert(n > 0 && n <= maxWords);
            requested += std::to_string(a) + ":" + std::to_string(n) + ";";
            ++transactions;
            if (transactions == 1) { result = 2; return false; }
            for (uint16_t i = 0; i < n; ++i) w[i] = 0x7FFF;
            result = 0; return true;
        });
        const std::string expected = preset == "bms" ? "400:15;415:4;" :
            preset == "thermal" ? "800:2;816:6;" : "338:6;536:9;816:6;";
        assert(requested == expected);
        assert(text.find("ERROR:") != std::string::npos);
        assert(text.find("u16=32767 i16=32767") != std::string::npos);
        assert(text.find("3276.7") == std::string::npos);
    }
    assert(snapshot("write", reader).find("ERROR:") == 0);
    std::cout << "EASUN diagnostic parser, raw decoding, transport failures and 65536 address boundaries passed\n";
}
