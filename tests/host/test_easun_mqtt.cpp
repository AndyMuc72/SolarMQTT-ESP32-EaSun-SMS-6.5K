#include "core/EasunMqttTelemetry.h"
#include <cassert>
#include <iostream>
#include <string>
using namespace EasunMqttTelemetry;
int main() {
    JsonDocument original;
    deserializeJson(original, R"({"Status":{"protocol":"MODBUS_EASUN_SMS"},"RawData":{"CommandAnswer":"0x0260 raw=17"},"LiveData":{"PV_Input_Voltage":1.7,"PV_Total_Input_Current":0.58,"PV_Total_Input_Power":1,"PV1_Input_Voltage":1.7,"PV1_Input_Current_Dedicated":0.59,"PV1_Input_Power":1,"AC_Out_Watt":-8,"AC_Out_VA":75,"AC_Out_Current_L1":0.33,"PV_Generation_Sum":64550}})");
    std::string before; serializeJson(original, before);
    JsonDocument outgoing; outgoing.set(original); normalize(outgoing);
    std::string after; serializeJson(original, after); assert(before == after);
    auto live=outgoing["LiveData"];
    for(const char *key : {"PV_Input_Voltage","PV_Total_Input_Current","PV_Total_Input_Power","PV1_Input_Voltage","PV1_Input_Current_Dedicated","PV1_Input_Power"}) assert(live[key].as<double>()==0);
    assert(std::fabs(original["LiveData"]["PV_Input_Voltage"].as<double>()-1.7)<1e-6);
    assert(std::fabs(original["LiveData"]["PV_Total_Input_Current"].as<double>()-0.58)<1e-6);
    assert(live["AC_Out_Watt"]==-8 && live["AC_Out_VA"]==75 && std::fabs(live["AC_Out_Current_L1"].as<double>()-0.33)<1e-6);
    assert(live["PV_Generation_Sum"]==64550);
    assert(std::string(outgoing["RawData"]["CommandAnswer"].as<const char*>())=="0x0260 raw=17");
    for(double v : {0.0,1.7,9.99,10.0,10.01,280.0}) for(double p : {0.0,1.0,9.99,10.0,10.01,150.0}) {
        JsonDocument d; d["Status"]["protocol"]="MODBUS_EASUN_SMS";
        d["LiveData"]["PV_Input_Voltage"]=v;d["LiveData"]["PV_Total_Input_Power"]=p;d["LiveData"]["PV_Total_Input_Current"]=0.58;
        normalize(d);
        assert(d["LiveData"]["PV_Input_Voltage"].as<double>()==(v<10?0:v));
        assert(d["LiveData"]["PV_Total_Input_Power"].as<double>()==(p<10?0:p));
        assert(d["LiveData"]["PV_Total_Input_Current"].as<double>()==((v<10||p<10)?0:0.58));
        assert(d["LiveData"]["PV2_Input_Voltage"].isNull());
        assert(d["LiveData"].as<JsonObject>().size()==3);
    }
    for(const char *protocol : {"MODBUS_POWMR","PI30",""}) {
        JsonDocument d;d.set(original);d["Status"]["protocol"]=protocol;
        normalize(d);assert(std::fabs(d["LiveData"]["PV_Input_Voltage"].as<double>()-1.7)<1e-6);
    }
    for(const char *section : {"DeviceData","EspData"}) {
        JsonDocument d;d[section][std::string(section)=="DeviceData"?"Protocol_ID":"detect_protocol_name"]="MODBUS_EASUN_SMS";
        d["LiveData"]["PV1_Input_Power"]=9;d["LiveData"]["PV1_Input_Current"]=0.5;
        normalize(d);assert(d["LiveData"]["PV1_Input_Current"].as<double>()==0);
        assert(d["LiveData"]["PV1_Input_Voltage"].isNull());
    }
    JsonDocument missing;normalize(missing);assert(missing["LiveData"].isNull());assert(missing.size()==0);
    std::cout<<"PASS: EASUN MQTT screenshot fixture, thresholds, daylight small currents, other protocols, raw/state preservation and missing fields\n";
}
