#include "deye.h"

#include <ctype.h>
#include <string.h>


const modbus_register_t *Deye::getLiveRegisters() const
{
    return registers_live;
}

const modbus_register_t *Deye::getStaticRegisters() const
{
    return registers_static;
}

const char *Deye::getName() const
{
    return _name;
}

bool Deye::retrieveModel(MODBUS_COM &mCom, char *modelBuffer, size_t bufferSize)
{
    if (modelBuffer == nullptr || bufferSize == 0)
    {
        return false;
    }

    modelBuffer[0] = '\0'; // Clear the buffer
    JsonDocument doc;
    JsonObject jsonObj = doc.to<JsonObject>(); // Create and get JsonObject
    modbus_register_info_t model_info = {
        .variant = &jsonObj,
        .registers = registers_device_serial,
        .array_size = sizeof(registers_device_serial) / sizeof(modbus_register_t),
        .curr_register = 0};

    for (size_t i = 0; i < model_info.array_size; i++)
    {
        mCom.parseModbusToJson(model_info, false);
        if (mCom.isAllRegistersRead(model_info))
        {
            const char *sn1 = doc["SN1"];
            const char *sn2 = doc["SN2"];
            const char *sn3 = doc["SN3"];
            const char *sn4 = doc["SN4"];
            const char *sn5 = doc["SN5"];
            char serial[16] = {};
            snprintf(serial, sizeof(serial), "%s%s%s%s%s", sn1, sn2, sn3, sn4, sn5);

            // The five serial-number reads alone are not a Deye signature:
            // unrelated Modbus devices can return valid values at 0x0003-0x0007.
            // Confirm them with Deye's rated-power and operation-mode registers.
            uint16_t ratingWords[2] = {};
            uint16_t operationMode = 0xFFFF;
            if (!mCom.readHoldingBlock(16, 2, ratingWords, 2) ||
                !mCom.readHoldingBlock(59, 1, &operationMode, 1))
            {
                return false;
            }

            size_t serialLength = strlen(serial);
            while (serialLength > 0 && (serial[serialLength - 1] == '\0' || serial[serialLength - 1] == ' '))
            {
                serial[--serialLength] = '\0';
            }
            if (serialLength < 8 || serialLength > 10)
            {
                return false;
            }
            for (size_t i = 0; i < serialLength; ++i)
            {
                if (!isalnum(static_cast<unsigned char>(serial[i])))
                {
                    return false;
                }
            }

            const uint32_t ratedPowerRaw = static_cast<uint32_t>(ratingWords[0]) |
                                           (static_cast<uint32_t>(ratingWords[1]) << 16);
            const float ratedPowerWatts = ratedPowerRaw / 10.0f;
            if (ratedPowerWatts < 500.0f || ratedPowerWatts > 50000.0f || operationMode > 4)
            {
                writeLog("Deye probe rejected serial=%s ratedPowerRaw=%lu mode=%u",
                         serial,
                         static_cast<unsigned long>(ratedPowerRaw),
                         static_cast<unsigned int>(operationMode));
                return false;
            }

            snprintf(modelBuffer, bufferSize, "%s", serial);
            return true;
        }
        delay(50);
    }
    return false;
}

// Define the size calculation after the arrays are defined
size_t Deye::getLiveRegistersCount() const
{
    return sizeof(registers_live) / sizeof(modbus_register_t);
}

size_t Deye::getStaticRegistersCount() const
{
    return sizeof(registers_static) / sizeof(modbus_register_t);
}
