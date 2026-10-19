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

    inline static const modbus_register_t registers_live[] = {
        // SMS-6.5KP USB-A map. The 0.1/0.01 scaling follows the model-specific
        // scans and ESPHome implementation; output power is raw watts.
        {0x0050, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16_ONE_DECIMAL, DESCR_AC_In_Voltage},
        {0x0052, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16_TWO_DECIMAL, DESCR_AC_In_Frequency},
        {0x0053, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16, DESCR_AC_In_Watt},
        {0x0058, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16_ONE_DECIMAL, DESCR_AC_Out_Voltage},
        {0x005A, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16_TWO_DECIMAL, DESCR_AC_Out_Frequency},
        {0x005D, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16, DESCR_Output_Load_Percent},
        {0x0085, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16, DESCR_AC_Out_Watt},
        {0x0155, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16_ONE_DECIMAL, DESCR_Battery_Voltage},
        {0x0156, MODBUS_TYPE_HOLDING, REGISTER_TYPE_INT16_ONE_DECIMAL, DESCR_Battery_Average_Current},
    };

    inline static const modbus_register_t registers_static[] = {
        {0x001B, MODBUS_TYPE_HOLDING, REGISTER_TYPE_U16, "Firmware_Version"},
    };
};

#endif // MODBUS_EASUN_SMS_H
