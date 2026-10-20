#ifndef MODBUS_EASUN_SMS_H
#define MODBUS_EASUN_SMS_H

#include <modbus/device/modbus_device.h>

// Read-only parser for the EASUN SMS USB-A/CVTE Modbus firmware family.
class EasunSms : public ModbusDevice
{
public:
    EasunSms() : ModbusDevice(_baudRate, _modbusAddr, _protocol) {}

    const modbus_register_t *getLiveRegisters() const override;
    const modbus_register_t *getStaticRegisters() const override;
    const char *getName() const override;
    bool retrieveModel(MODBUS_COM &mCom, char *modelBuffer, size_t bufferSize) override;
    size_t getLiveRegistersCount() const override;
    size_t getStaticRegistersCount() const override;

private:
    static constexpr long _baudRate = 9600;
    static constexpr uint32_t _modbusAddr = 1;
    static constexpr protocol_type_t _protocol = MODBUS_EASUN_SMS;
    inline static constexpr const char *_name = "EASUN SMS / CVTE";

    // Stable 14-byte ASCII serial block observed on three SMS-6.5KP-W-E units.
    // Keep the signature narrow; manual selection remains available for related
    // SMS revisions that use a different prefix.
    static constexpr uint16_t kSerialStart = 0x0000;
    static constexpr uint16_t kSerialWordCount = 7;
    inline static constexpr char kIdentityPrefix[] = "803096";
    static constexpr uint16_t kFaultBlockStart = 0x0046;
    static constexpr uint16_t kFaultBlockCount = 4;
    static constexpr uint16_t kAcLiveBlockStart = 0x0050;
    static constexpr uint16_t kAcLiveBlockCount = 14;
    static constexpr uint16_t kBatteryBlockStart = 0x0154;
    static constexpr uint16_t kBatteryBlockCount = 4;
    static constexpr uint16_t kOutputBlockStart = 0x0218;
    static constexpr uint16_t kOutputBlockCount = 9;
    static constexpr uint16_t kPvTotalBlockStart = 0x0260;
    static constexpr uint16_t kPvTotalBlockCount = 15;
    static constexpr uint16_t kPv1BlockStart = 0x0270;
    static constexpr uint16_t kPv1BlockCount = 13;
    static constexpr uint16_t kFanBlockStart = 0x0320;
    static constexpr uint16_t kFanBlockCount = 2;
    static constexpr uint16_t kExperimentalTemperatureBlockStart = 0x00A4;
    static constexpr uint16_t kExperimentalTemperatureBlockCount = 3;
    static constexpr uint16_t kThermalBlockStart = 0x0330;
    static constexpr uint16_t kThermalBlockCount = 6;

    inline static const modbus_register_t registers_live[] = {
        // SMS-6.5KP USB-A map. The 0.1/0.01 scaling follows the model-specific
        // scans and ESPHome implementation; output power is raw watts.
        {0x0050, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16_ONE_DECIMAL, DESCR_AC_In_Voltage, 0, {}, nullptr, kAcLiveBlockStart, kAcLiveBlockCount},
        {0x0052, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16_TWO_DECIMAL, DESCR_AC_In_Frequency, 0, {}, nullptr, kAcLiveBlockStart, kAcLiveBlockCount},
        {0x0053, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16, DESCR_AC_In_Watt, 0, {}, nullptr, kAcLiveBlockStart, kAcLiveBlockCount},
        {0x0058, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16_ONE_DECIMAL, DESCR_AC_Out_Voltage, 0, {}, nullptr, kAcLiveBlockStart, kAcLiveBlockCount},
        {0x005A, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16_TWO_DECIMAL, DESCR_AC_Out_Frequency, 0, {}, nullptr, kAcLiveBlockStart, kAcLiveBlockCount},
        {0x005D, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16, DESCR_Output_Load_Percent, 0, {}, nullptr, kAcLiveBlockStart, kAcLiveBlockCount},
        {0x0085, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16, DESCR_AC_Out_Watt},
        // CVTE telemetry map (table based; validate against the WiFi-labelled unit).
        // PR #163 reports these three registers as experimental temperature
        // candidates. Keep their raw values and avoid assigning components.
        {0x00A4, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, "Temperature_A_Experimental", 0, {}, nullptr, kExperimentalTemperatureBlockStart, kExperimentalTemperatureBlockCount},
        {0x00A5, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, "Temperature_B_Experimental", 0, {}, nullptr, kExperimentalTemperatureBlockStart, kExperimentalTemperatureBlockCount},
        {0x00A6, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, "Temperature_C_Experimental", 0, {}, nullptr, kExperimentalTemperatureBlockStart, kExperimentalTemperatureBlockCount},
        {0x0046, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, "Fault_Code_1", 0, {}, nullptr, kFaultBlockStart, kFaultBlockCount},
        {0x0047, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, "Fault_Code_2", 0, {}, nullptr, kFaultBlockStart, kFaultBlockCount},
        {0x0048, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, "Alarm_Code_1", 0, {}, nullptr, kFaultBlockStart, kFaultBlockCount},
        {0x0049, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, "Alarm_Code_2", 0, {}, nullptr, kFaultBlockStart, kFaultBlockCount},
        {0x0144, MODBUS_TYPE_HOLDING, REGISTER_TYPE_CUSTOM_VAL_NAME, "Operating_State", 0, {.bitfield = {
            "Power On", "Initialization", "Standby", "AC Mode", "Solar Mode", "Battery Mode",
            "Generator Mode", "Fault", "Power Off", "Factory", "Upgrade"}}, nullptr},
        {0x0152, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16, DESCR_Battery_Percent},
        {0x0154, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_ONE_DECIMAL, "Battery_Bus_Voltage", 0, {}, nullptr, kBatteryBlockStart, kBatteryBlockCount},
        {0x0155, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16_ONE_DECIMAL, DESCR_Battery_Voltage, 0, {}, nullptr, kBatteryBlockStart, kBatteryBlockCount},
        {0x0156, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_ONE_DECIMAL, DESCR_Battery_Average_Current, 0, {}, nullptr, kBatteryBlockStart, kBatteryBlockCount},
        {0x0157, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_ONE_DECIMAL, DESCR_Battery_Charge_Current, 0, {}, nullptr, kBatteryBlockStart, kBatteryBlockCount},
        {0x0176, MODBUS_TYPE_HOLDING, REGISTER_TYPE_CUSTOM_VAL_NAME, DESCR_Inverter_Charge_State, 0, {.bitfield = {
            "Not Charging", "CC/CV", "Float", "Equalization"}}, nullptr},
        {0x01B1, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_TWO_DECIMAL, DESCR_AC_In_Current_L1},
        {0x0210, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, "Output_Priority"},
        {0x0211, MODBUS_TYPE_HOLDING, REGISTER_TYPE_CUSTOM_VAL_NAME, "Run_Mode", 0, {.bitfield = {"APP", "UPS", "GEN"}}, nullptr},
        {0x021A, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_TWO_DECIMAL, DESCR_AC_Out_Current_L1, 0, {}, nullptr, kOutputBlockStart, kOutputBlockCount},
        {0x021C, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16, "AC_Output_Active_Power", 0, {}, nullptr, kOutputBlockStart, kOutputBlockCount},
        {0x021D, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16, DESCR_AC_Out_VA, 0, {}, nullptr, kOutputBlockStart, kOutputBlockCount},
        {0x0220, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_ONE_DECIMAL, DESCR_AC_Out_Percent_L1, 0, {}, nullptr, kOutputBlockStart, kOutputBlockCount},
        // One physical tracker: total PV and PV1 are read; PV2 (0x0280-0x028C)
        // is intentionally omitted from the polling list for this model.
        {0x0260, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_ONE_DECIMAL, DESCR_PV_Input_Voltage, 0, {}, nullptr, kPvTotalBlockStart, kPvTotalBlockCount},
        {0x0261, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U32_HIGH_FIRST_TWO_DECIMAL, "PV_Total_Input_Current", 0, {}, nullptr, kPvTotalBlockStart, kPvTotalBlockCount},
        {0x0263, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U32_HIGH_FIRST, DESCR_PV_Total_Input_Power, 0, {}, nullptr, kPvTotalBlockStart, kPvTotalBlockCount},
        {0x0265, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, "PV_Tracking_State", 0, {}, nullptr, kPvTotalBlockStart, kPvTotalBlockCount},
        {0x0266, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, "PV_Charge_State", 0, {}, nullptr, kPvTotalBlockStart, kPvTotalBlockCount},
        {0x0267, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U32_HIGH_FIRST_TEN, DESCR_PV_Generation_Day, 0, {}, nullptr, kPvTotalBlockStart, kPvTotalBlockCount},
        {0x0269, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U32_HIGH_FIRST_TEN, DESCR_PV_Generation_Month, 0, {}, nullptr, kPvTotalBlockStart, kPvTotalBlockCount},
        {0x026B, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U32_HIGH_FIRST_TEN, DESCR_PV_Generation_Year, 0, {}, nullptr, kPvTotalBlockStart, kPvTotalBlockCount},
        {0x026D, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U32_HIGH_FIRST_TEN, DESCR_PV_Generation_Sum, 0, {}, nullptr, kPvTotalBlockStart, kPvTotalBlockCount},
        {0x0270, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_ONE_DECIMAL, DESCR_PV1_Input_Voltage, 0, {}, nullptr, kPv1BlockStart, kPv1BlockCount},
        {0x0271, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_TWO_DECIMAL, "PV1_Input_Current_Dedicated", 0, {}, nullptr, kPv1BlockStart, kPv1BlockCount},
        {0x0272, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16, DESCR_PV1_Input_Power, 0, {}, nullptr, kPv1BlockStart, kPv1BlockCount},
        {0x0273, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, "PV1_Tracking_State", 0, {}, nullptr, kPv1BlockStart, kPv1BlockCount},
        {0x0274, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, "PV1_Charge_State", 0, {}, nullptr, kPv1BlockStart, kPv1BlockCount},
        {0x0275, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U32_HIGH_FIRST_TEN, "PV1_Generation_Day_Wh", 0, {}, nullptr, kPv1BlockStart, kPv1BlockCount},
        {0x0277, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U32_HIGH_FIRST_TEN, "PV1_Generation_Month_Wh", 0, {}, nullptr, kPv1BlockStart, kPv1BlockCount},
        {0x0279, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U32_HIGH_FIRST_TEN, "PV1_Generation_Year_Wh", 0, {}, nullptr, kPv1BlockStart, kPv1BlockCount},
        {0x027B, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U32_HIGH_FIRST_TEN, "PV1_Generation_Total_Wh", 0, {}, nullptr, kPv1BlockStart, kPv1BlockCount},
        {0x0320, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, DESCR_Fan_Speed, 0, {}, nullptr, kFanBlockStart, kFanBlockCount},
        {0x0321, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, DESCR_Fan_Lock_Status, 0, {}, nullptr, kFanBlockStart, kFanBlockCount},
        // Provisional target-specific scale: the connected WiFi-labelled unit
        // returns 310/340/480/480/250, which are plausible as tenths of a
        // degree. The public CVTE table says integer Celsius; keep this marked
        // as provisional until raw values can be checked against hardware.
        {0x0330, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_ONE_DECIMAL, DESCR_Tracker_Temperature, 0, {}, nullptr, kThermalBlockStart, kThermalBlockCount},
        {0x0331, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_ONE_DECIMAL, DESCR_Inverter_Temperature, 0, {}, nullptr, kThermalBlockStart, kThermalBlockCount},
        {0x0332, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_ONE_DECIMAL, DESCR_MPPT1_Charger_Temperature, 0, {}, nullptr, kThermalBlockStart, kThermalBlockCount},
        {0x0333, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_ONE_DECIMAL, DESCR_MPPT2_Charger_Temperature, 0, {}, nullptr, kThermalBlockStart, kThermalBlockCount},
        {0x0334, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_ONE_DECIMAL, "Ambient_Temperature", 0, {}, nullptr, kThermalBlockStart, kThermalBlockCount},
        {0x0335, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_ONE_DECIMAL, "Discharge_Temperature", 0, {}, nullptr, kThermalBlockStart, kThermalBlockCount},
    };

    inline static const modbus_register_t registers_static[] = {
        {0x001B, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, "Firmware_Version"},
    };
};

#endif // MODBUS_EASUN_SMS_H
