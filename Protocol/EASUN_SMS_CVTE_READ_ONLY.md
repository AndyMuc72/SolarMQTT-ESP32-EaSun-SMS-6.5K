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
| `0x005D` | legacy load percentage | signed integer percent | model example; confirm on target |
| `0x0085` | `AC_Out_Watt` | signed watts | corrected empirical mapping; raw watts |
| `0x0155` | `Battery_Voltage` | unsigned ×0.1 V | corrected empirical mapping; live voltage |
| `0x0156` | `Battery_Average_Current` | signed ×0.1 A | corrected empirical mapping; negative is discharge |
| `0x001B` | `Firmware_Version` | unsigned raw code | model-specific example |

## Expanded telemetry (CVTE table, one MPPT)

The following read-only telemetry was added from the public EASUN/CVTE map and SMS ESPHome package. It is exposed in `/livejson` and MQTT. Register reads that the inverter rejects simply do not produce a value; they do not stop the other fields. Public implementation identifies SMS-6.2KP-W as validated and SMS-6.5KP-W-E as under validation, so these addresses are table-derived candidates for the WiFi-labelled SMS-6.5KP, not confirmed physical readings from this exact unit.

| Address | Published field | Decode |
|---:|---|---|
| `0x0046–0x0049` | `Fault_Code_1/2`, `Alarm_Code_1/2` | raw words |
| `0x0144` | `Operating_State` | 0 Power On, 1 Initialization, 2 Standby, 3 AC, 4 Solar, 5 Battery, 6 Generator, 7 Fault, 8 Off, 9 Factory, 10 Upgrade |
| `0x0152` | `Battery_Percent` | signed integer % |
| `0x0154` | `Battery_Bus_Voltage` | signed ×0.1 V |
| `0x0157` | `Battery_Charge_Current` | signed ×0.1 A |
| `0x0176` | `Inverter_Charge_State` | 0 not charging, 1 CC/CV, 2 float, 3 equalization |
| `0x01B1` | `AC_In_Current_L1` | signed ×0.01 A (grid/mains current) |
| `0x0210–0x0211` | `Output_Priority`, `Run_Mode` | raw priority; 0 APP, 1 UPS, 2 GEN |
| `0x021A` | `AC_Out_Current_L1` | signed ×0.01 A |
| `0x021C–0x021D` | `AC_Output_Active_Power`, `AC_Out_VA` | signed watts / apparent power |
| `0x0220` | `AC_Out_Percent_L1` | signed ×0.1 % |
| `0x0260` | `PV_Input_Voltage` | signed ×0.1 V |
| `0x0261–0x0262` | `PV_Total_Input_Current` | high-word-first U32 ×0.01 A |
| `0x0263–0x0264` | `PV_Total_Input_Power` | high-word-first U32 W |
| `0x0265–0x0266` | PV tracker / charge state | raw words |
| `0x0267–0x026E` | PV total day/month/year/lifetime generation | high-word-first U32 ×10 Wh |
| `0x0270–0x0274` | PV1 voltage/current/power and states | ×0.1 V, ×0.01 A, W, raw states |
| `0x0275–0x027C` | PV1 day/month/year/lifetime generation | high-word-first U32 ×10 Wh |
| `0x0320–0x0321` | `Fan_Speed`, `Fan_Lock_Status` | speed %; non-zero status means blocked per table |
| `0x0330–0x0335` | tracker, inverter, charger, charger 2, ambient, discharge temperatures | signed °C |

This label specifies one MPPT. The parser therefore reads the total-PV and PV1 range (`0x0260–0x027C`) and deliberately does not poll PV2 (`0x0280–0x028C`). The values in the total and PV1 groups may be duplicates on single-tracker firmware. `0x01B1` is the inverter's measured grid input current; a separate external CT/meter current was not found in the CVTE read-only map, so the grid current must not be described as an external CT reading.

The source map and sensor definitions are in [JamesMacieira/ESPHome-Easun-SMS-Inverter](https://github.com/JamesMacieira/ESPHome-Easun-SMS-Inverter) (`modbus/json/protocol.json`, `modules/wf_pv.yaml`, `modules/wf_thermal.yaml`, `modules/wf_workmode.yaml`, `modules/wf_bat.yaml`, `modules/wf_simplify.yaml`). Scaling and multiword decoding follow those definitions. The model-specific corrections above follow [syssi/esphome-smg-ii PR #163](https://github.com/syssi/esphome-smg-ii/pull/163).

### Test status

**Firmware build:** recorded by the GitHub Actions workflow for the Wemos D1 mini ESP32 target. A green check confirms compilation only. The WiFi-labelled inverter has not yet been physically checked register-by-register against its LCD; do that before treating table-derived fields as confirmed. The firmware continues to use Modbus read function 0x03 only.

The `0x0159` nominal battery voltage and `0x015D` configuration value are deliberately not published as live telemetry. PV registers `0x0090/0x0091`, battery SOC, temperatures, warnings, and operating state are also omitted until a mapping is confirmed for the attached `iSolar-SMS-6.5KP-Wifi` unit. In particular, `0x005B` is not used for output power because it can read zero at low loads.

The strongest available register evidence is [syssi/esphome-smg-ii PR #163](https://github.com/syssi/esphome-smg-ii/pull/163), which reports scans of three physical SMS-6.5KP-W-E units. The attached WiFi-labelled model may have a different firmware revision, so compare readings with the inverter display before relying on them operationally.

## Development logging

Define `MODBUS_EASUN_SMS_DEBUG` for a development build to log function 0x03 request parameters and CRC, returned holding-register values, and the decoded field, raw value, signed interpretation, and scale. The macro is disabled in normal builds.
