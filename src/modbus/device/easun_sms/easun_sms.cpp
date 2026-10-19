#include "easun_sms.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

const modbus_register_t *EasunSms::getLiveRegisters() const
{
    return registers_live;
}

const modbus_register_t *EasunSms::getStaticRegisters() const
{
    return registers_static;
}

const char *EasunSms::getName() const
{
    return _name;
}

bool EasunSms::retrieveModel(MODBUS_COM &mCom, char *modelBuffer, size_t bufferSize)
{
    if (modelBuffer == nullptr || bufferSize == 0)
    {
        return false;
    }

    modelBuffer[0] = '\0';
    uint16_t serialWords[kSerialWordCount] = {};
    if (!mCom.readHoldingBlock(kSerialStart, kSerialWordCount, serialWords, kSerialWordCount))
    {
        return false;
    }

    char serial[kSerialWordCount * 2 + 1] = {};
    size_t position = 0;
    for (size_t i = 0; i < kSerialWordCount; ++i)
    {
        const char chars[] = {
            static_cast<char>((serialWords[i] >> 8) & 0xFF),
            static_cast<char>(serialWords[i] & 0xFF),
        };
        for (char value : chars)
        {
            if (!isdigit(static_cast<unsigned char>(value)) || position + 1 >= sizeof(serial))
            {
                return false;
            }
            serial[position++] = value;
        }
    }
    serial[position] = '\0';

    constexpr size_t prefixLength = sizeof(kIdentityPrefix) - 1;
    if (position != kSerialWordCount * 2 || strncmp(serial, kIdentityPrefix, prefixLength) != 0)
    {
        return false;
    }

    snprintf(modelBuffer, bufferSize, "EASUN SMS-6.5KP (%s)", serial);
    return true;
}

size_t EasunSms::getLiveRegistersCount() const
{
    return sizeof(registers_live) / sizeof(registers_live[0]);
}

size_t EasunSms::getStaticRegistersCount() const
{
    return sizeof(registers_static) / sizeof(registers_static[0]);
}
