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
| `0x005D` | `Output_Load_Percent_005D_Reference` | signed raw integer | target read 21 conflicts with 2.1% from `0x0220`; diagnostic only |
| `0x0085` | `AC_Out_Watt_0085_Reference` | signed raw watts | corrected on three W-E units; reads 0 on this WiFi/FW8 target at the observed 128 W load |
| `0x00A4–0x00A6` | `Temperature_A/B/C_Experimental` | raw unsigned words; no assumed scale or component mapping | experimental candidates in PR #163 |
| `0x0155` | `Battery_Voltage` | unsigned ×0.1 V | corrected empirical mapping; matches target and Pylontech voltage |
| `0x0156` | `Battery_Current` | signed ×0.1 A | target value −4.9 A matches summed Pylontech current ≈−4.98 A; negative is discharge |
| `0x0156` | `Battery_Load` / `Battery_Average_Current` | signed ×0.1 A | aliases for energy integration and compatibility |
| `0x001B` | `Firmware_Version` | unsigned raw code | model-specific example |

## Expanded telemetry (CVTE table, one MPPT)

The following read-only telemetry was added from the public EASUN/CVTE map and SMS ESPHome package. It is exposed in `/livejson` and MQTT. Register reads that the inverter rejects simply do not produce a value; they do not stop the other fields. Public implementation identifies SMS-6.2KP-W as validated and SMS-6.5KP-W-E as under validation, so these addresses are table-derived candidates for the WiFi-labelled SMS-6.5KP, not confirmed physical readings from this exact unit.

| Address | Published field | Decode |
|---:|---|---|
| `0x0046–0x0049` | `Fault_Code_1/2`, `Alarm_Code_1/2` | raw words |
| `0x0144` | `Operating_State` | 0 Power On, 1 Initialization, 2 Standby, 3 AC, 4 Solar, 5 Battery, 6 Generator, 7 Fault, 8 Off, 9 Factory, 10 Upgrade |
| `0x0152` | `Battery_Percent`, `Inverter_Battery_Percent` | signed integer %; inverter estimate only |
| `0x0154` | `DC_Bus_Voltage` | signed ×0.1 V |
| `0x0157` | `Battery_Current_0157_Experimental` | signed ×0.1 A; name/meaning not confirmed |
| `0x0190–0x0192` | BMS protocol, communication state, pack ID | raw words; status labels 0 ID Search, 1 ID Remote Lock, 2 ID Locked |
| `0x0193–0x0194` | `BMS_Voltage`, `BMS_Current` | signed ×0.1 V / A |
| `0x0195–0x0197` | `BMS_Temperature`, `BMS_SOC`, `BMS_SOH` | signed integer °C / % / % |
| `0x0198–0x0199` | current and full-charge capacity raw values | unsigned raw counts; reference map says 10 mAh per count |
| `0x0176` | `Inverter_Charge_State` | 0 not charging, 1 CC/CV, 2 float, 3 equalization |
| `0x01B1` | `AC_In_Current_L1` | signed ×0.01 A (grid/mains current) |
| `0x0210–0x0211` | `Output_Priority`, `Run_Mode` | raw priority; 0 APP, 1 UPS, 2 GEN |
| `0x021A` | `AC_Out_Current_L1` | signed ×0.01 A |
| `0x021C–0x021D` | `AC_Out_Watt` / `AC_Output_Active_Power`, `AC_Out_VA` | signed watts / apparent power |
| `0x0220` | `Output_Load_Percent`, `AC_Out_Percent_L1` | signed ×0.1 % |
| `0x0260` | `PV_Input_Voltage` | signed ×0.1 V |
| `0x0261–0x0262` | `PV_Total_Input_Current` | high-word-first U32 ×0.01 A |
| `0x0263–0x0264` | `PV_Total_Input_Power` | high-word-first U32 W |
| `0x0265–0x0266` | PV tracker / charge state | raw words |
| `0x0267–0x026E` | PV total day/month/year/lifetime generation | high-word-first U32 ×10 Wh |
| `0x0270–0x0274` | PV1 voltage/current/power and states | ×0.1 V, ×0.01 A, W, raw states |
| `0x0275–0x027C` | PV1 day/month/year/lifetime generation | high-word-first U32 ×10 Wh |
| `0x0320–0x0321` | `Fan_Speed`, `Fan_Lock_Status` | speed %; non-zero status means blocked per table |
| `0x0330–0x0335` | tracker, inverter, charger, charger 2, ambient, discharge temperatures | signed ×0.1 °C, provisional for this WiFi-labelled unit |

The public CVTE register sheet lists these temperatures as signed integer °C (scale 1), and its ESPHome package decodes them without a decimal scale. That package reports SMS-6.2KP-W as validated and SMS-6.5KP-W-E as under validation. On the attached WiFi-labelled SMS-6.5KP, the observed raw values are all multiples of ten (for example 310, 340, 480 and 250); dividing by ten gives plausible temperatures (31.0, 34.0, 48.0 and 25.0 °C). Solar2MQTT therefore applies ×0.1 provisionally for this target. Confirm against raw register values and an independent temperature reading before treating the scale as final.

The BMS block at `0x0190–0x0199` is added as a read-only diagnostic. Compare `BMS_SOC` (`0x0196`) with the inverter's `Battery_Percent` (`0x0152`) and the battery's own app. A status of `0` is labelled ID Search, `1` ID Remote Lock, and `2` ID Locked by the ESPHome reference definitions; validate what this status means on the WiFi/FW8 target. `BMS_Voltage` and `BMS_Current` should be compared with `Battery_Voltage` and `Battery_Current`. The raw current/full capacity counts at `0x0198/0x0199` can provide a ratio check; the reference map assigns a 10 mAh/count scale. This register block has not yet been confirmed on this exact unit, and the fields may be unavailable if the inverter has no active BMS link.

PR #163 also labels `0x00A4–0x00A6` as experimental temperature candidates. It describes the three values as the same at low load and says their exact component assignment is unknown. Solar2MQTT exposes these as raw unsigned words named `Temperature_A/B/C_Experimental`; it does not apply a scale or present them as confirmed sensor locations. Compare these values with the app before relying on them.

This label specifies one MPPT. The parser therefore reads the total-PV and PV1 range (`0x0260–0x027C`) and deliberately does not poll PV2 (`0x0280–0x028C`). The values in the total and PV1 groups may be duplicates on single-tracker firmware. `0x01B1` is the inverter's measured grid input current; a separate external CT/meter current was not found in the CVTE read-only map, so the grid current must not be described as an external CT reading.

The source map and sensor definitions are in [JamesMacieira/ESPHome-Easun-SMS-Inverter](https://github.com/JamesMacieira/ESPHome-Easun-SMS-Inverter) (`modbus/json/protocol.json`, `modules/wf_pv.yaml`, `modules/wf_thermal.yaml`, `modules/wf_workmode.yaml`, `modules/wf_bat.yaml`, `modules/wf_simplify.yaml`). Scaling and multiword decoding follow those definitions. The model-specific corrections above follow [syssi/esphome-smg-ii PR #163](https://github.com/syssi/esphome-smg-ii/pull/163).

### Test status

**Firmware build:** recorded by the GitHub Actions workflow for the Wemos D1 mini ESP32 target. A green check confirms compilation only. The WiFi-labelled inverter has not yet been physically checked register-by-register against its LCD or BMS values; do that before treating table-derived fields as confirmed. The firmware continues to use Modbus read function 0x03 only.

The `0x0159` nominal battery voltage and `0x015D` configuration value are deliberately not published as live telemetry. PV registers `0x0090/0x0091` are not used because scans report that they do not exist on the W-E firmware. Battery SOC, warnings, operating state, and CVTE thermal registers are table-derived and need comparison with the attached `iSolar-SMS-6.5KP-Wifi` unit. In particular, `0x005B` is not used for output power because it can read zero at low loads.

The strongest available register evidence is [syssi/esphome-smg-ii PR #163](https://github.com/syssi/esphome-smg-ii/pull/163), which reports scans of three physical SMS-6.5KP-W-E units. The BMS register definitions also appear in [JamesMacieira's ESPHome CVTE implementation](https://github.com/JamesMacieira/ESPHome-Easun-SMS-Inverter/blob/main/modules/wf_bms.yaml); that project marks the 6.5KP-W-E as under validation. The attached WiFi-labelled model may have a different firmware revision, so compare readings with the inverter display and battery app before relying on them operationally.

## Target-specific checks (WiFi-labelled unit, firmware 8)

The user's current target snapshot provides additional evidence that takes priority over the W-E scans where they disagree:

| Register / field | Observed value | Assessment |
|---|---:|---|
| `0x0155` / `Battery_Voltage` | 48.2–48.4 V; Pylontech ≈48.45 V | Confirmed measured battery voltage |
| `0x0156` / `Battery_Current` | −4.9 A; seven Pylontech modules total ≈−4.98 A | Strongly confirmed signed battery current |
| `0x0152` / `Battery_Percent` | 51%; Pylontech average ≈19% | Inverter estimate only; not authoritative BMS SOC and unsuitable for SOC automations |
| `0x0154` / `DC_Bus_Voltage` | ≈399.8 V | Plausible internal high-voltage DC link; it is not the 48 V battery bus |
| `0x021A` / `AC_Out_Current_L1` | 1.45 A | Consistent with the following apparent-power reading |
| `0x021C` / `AC_Out_Watt` | 128 W | Target-specific active output power; use this in the common output field |
| `0x021D` / `AC_Out_VA` | 333 VA | Strongly validated: 230 V × 1.45 A ≈333.5 VA |
| `0x0220` / `Output_Load_Percent` | 2.1% | Consistent with 128 W on a 6.5 kW inverter; supersedes `0x005D` for this target |
| `0x005D` / reference field | 21 | Does not agree with 2.1%; retained as a diagnostic value only |
| `0x0085` / reference field | 0 W while `0x021C` reports 128 W | W-E correction does not match this firmware at this operating point; retained for comparison |
| `0x0157` / reference field | −4.9 A | Plausible numeric value, but the “charge current” label is not confirmed; exposed as experimental |

The Modbus parser publishes repeated aliases for selected raw registers so existing MQTT consumers can keep their field names while the new canonical values and diagnostic references remain visible. All reads remain function 0x03 only.

## Development logging

Define `MODBUS_EASUN_SMS_DEBUG` for a development build to log function 0x03 request parameters and CRC, returned holding-register values, and the decoded field, raw value, signed interpretation, and scale. The macro is disabled in normal builds.
