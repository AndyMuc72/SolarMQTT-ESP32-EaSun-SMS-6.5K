#pragma once
#include <ArduinoJson.h>
#include <cmath>
#include <cstring>

// Presentation policy for outgoing EASUN MQTT snapshots only.
// Never mutate SolarState, diagnostic replies or accumulated energy counters.
namespace EasunMqttTelemetry {
inline bool below(JsonVariantConst value, double threshold) {
    return value.is<double>() && std::isfinite(value.as<double>()) &&
           std::fabs(value.as<double>()) < threshold;
}
inline void normalize(JsonDocument &snapshot) {
    const char *protocol = snapshot["Status"]["protocol"] | "";
    if (!*protocol) protocol = snapshot["DeviceData"]["Protocol_ID"] | "";
    if (!*protocol) protocol = snapshot["EspData"]["detect_protocol_name"] | "";
    if (std::strcmp(protocol, "MODBUS_EASUN_SMS") != 0) return;
    JsonObject live = snapshot["LiveData"].as<JsonObject>();
    if (live.isNull()) return;
    struct Channel { const char *voltage; const char *power; const char *current; const char *alias; };
    const Channel channels[] = {
        {"PV_Input_Voltage", "PV_Total_Input_Power", "PV_Total_Input_Current", nullptr},
        {"PV1_Input_Voltage", "PV1_Input_Power", "PV1_Input_Current_Dedicated", "PV1_Input_Current"},
        {"PV2_Input_Voltage", "PV2_Input_Power", "PV2_Input_Current_Dedicated", "PV2_Input_Current"},
    };
    for (const auto &channel : channels) {
        const bool lowVoltage = below(live[channel.voltage], 10.0);
        const bool lowPower = below(live[channel.power], 10.0);
        if (lowVoltage) live[channel.voltage] = 0;
        if (lowPower) live[channel.power] = 0;
        // No arbitrary current cutoff: e.g. 0.58 A at 280 V is real power.
        if (lowVoltage || lowPower) {
            if (live[channel.current].is<double>()) live[channel.current] = 0;
            if (channel.alias && live[channel.alias].is<double>()) live[channel.alias] = 0;
        }
    }
}
} // namespace EasunMqttTelemetry
