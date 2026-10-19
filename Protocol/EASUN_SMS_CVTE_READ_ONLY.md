# EASUN SMS / CVTE read-only support

Solar2MQTT has a dedicated `MODBUS_EASUN_SMS` parser for the EASUN iSolar-SMS 6.5KP family using the USB-A COM interface. Communication is Modbus RTU at 9600 baud, 8N1, slave 1. The parser sends holding-register reads only; it contains no inverter write commands or wake-up frame.

## Detection

Automatic detection checks the seven-word ASCII serial block at `0x0000–0x0006`. It accepts only a 14-digit string beginning with `803096`, a prefix found in the scans of three SMS-6.5KP-W-E devices. The EASUN probe runs before generic Deye probing. Related firmware may use another serial prefix; select **EASUN SMS / CVTE (read-only)** manually in Device Settings in that case.

Deye detection now requires a printable serial, a plausible rated-power value from registers `0x0010–0x0011` (500–50,000 W), and an operation-mode value `0–4` at `0x003B`. A successful generic Modbus read alone is insufficient.

## Mapped fields

| Holding register | Solar2MQTT field | Decode | Evidence |
|---:|---|---|---|
| `0x0050` | `AC_In_Voltage` | unsigned ×0.1 V | SMS-6.5KP-W-E reference map |
| `0x0052` | `AC_In_Frequency` | unsigned ×0.01 Hz | SMS-6.5KP-W-E reference map |
| `0x0053` | `AC_In_Watt` | signed watts | SMS-6.5KP-W-E reference map |
| `0x0058` | `AC_Out_Voltage` | unsigned ×0.1 V | SMS-6.5KP-W-E reference map |
| `0x005A` | `AC_Out_Frequency` | unsigned ×0.01 Hz | SMS-6.5KP-W-E reference map |
| `0x005D` | `Output_Load_Percent` | signed integer percent | model example; confirm on target |
| `0x0085` | `AC_Out_Watt` | signed watts | corrected empirical mapping; raw watts |
| `0x0155` | `Battery_Voltage` | unsigned ×0.1 V | corrected empirical mapping; live voltage |
| `0x0156` | `Battery_Average_Current` | signed ×0.1 A | corrected empirical mapping; negative is discharge |
| `0x001B` | `Firmware_Version` | unsigned raw code | model-specific example |

The `0x0159` nominal battery voltage and `0x015D` configuration value are deliberately not published as live telemetry. PV registers `0x0090/0x0091`, battery SOC, temperatures, warnings, and operating state are also omitted until a mapping is confirmed for the attached `iSolar-SMS-6.5KP-Wifi` unit. In particular, `0x005B` is not used for output power because it can read zero at low loads.

The strongest available register evidence is [syssi/esphome-smg-ii PR #163](https://github.com/syssi/esphome-smg-ii/pull/163), which reports scans of three physical SMS-6.5KP-W-E units. The attached WiFi-labelled model may have a different firmware revision, so compare readings with the inverter display before relying on them operationally.

## Development logging

Define `MODBUS_EASUN_SMS_DEBUG` for a development build to log function 0x03 request parameters and CRC, returned holding-register values, and the decoded field, raw value, signed interpretation, and scale. The macro is disabled in normal builds.
