# EASUN 10: validation status

## Scope

Public version: 0.1.0-easun.10. Its functional code was tested locally as 0.1.0-easun.10-cvte-test.2. The publication preparation changes the version label and documentation, not firmware behavior. Exact public-image test results and hashes are attached to the release.

## Software tests

- Native C++ test of the actual MQTT normalization function with ArduinoJson: supplied night fixture (1.7 V / 0.58 A / 1 W), PV1 alias, threshold boundaries, valid small daytime currents, other protocols, unchanged raw/AC/energy values, and absent fields.
- Existing dashboard regression: 5,029 assertions, including genuine zero watts and separate VA display.
- Bounded EASUN diagnostic tests cover signed raw values, unavailable markers, exact preset ranges, timeouts/partial errors and all 65,536 address boundaries.
- CI runs native regressions on Windows, Linux and macOS, and compiles all five configured boards. Only the Wemos image is supplied in this release; other successful compile checks are not hardware validation.
- Release-image checks verify ESP checksums/SHA256, empty public NVS, application/merged-image consistency and OTA partition capacity. Public Wi-Fi defaults must be empty.

## Local serial test of the functional candidate

ESP32-D0WD-V3 revision 3.0, 4 MB, wemos_d1_mini32. The active OTA slot was determined from the partition table and valid OTA-selection CRC. Only the application was written; settings were not overwritten. No full backup was requested.

The .10-cvte-test.2 app was hash-verified and observed for 180 seconds: correct version, one deliberate initial ROM reset, no additional resets, panic, watchdog, brownout or heap corruption, and successful backlog initialization. Post-initialization free-heap samples were 162944 / 161652 / 162000 bytes.

This serial test ran in access-point mode. It does not validate live broker delivery, router connectivity, web OTA, inverter measurements, BMS communication or long-term stability. Native tests verify the MQTT transformation independently of the unavailable broker connection.

## Register comparison

Reference: https://github.com/JamesMacieira/ESPHome-Easun-SMS-Inverter at 3b45acde70d49679fe9b9b3287096d0f06a687b8. The source author validates SMS-6.2KP-W; SMS-6.5KP-W-E remains under validation.

The supplied CVTE_Modbus_v1.20.xlsx is byte-identical to the reference document (SHA256 0cd88f12ec2774371093747da355fe1b6102411f164b350c1305de86049b94d2).

- 0x021A: signed /100 A in both implementations; no new scale correction established.
- 0x021B: signed /100 A, explicitly reserved. Experimental output only.
- 0x021C: active watts. 0x021D: apparent power; retain VA despite the spreadsheet's inconsistent W unit label.
- 0x0218/0x0219: additional output voltage/frequency references, not replacements for established readings.
- PV addresses/scales agree. Raw ×0.01 kWh in the reference equals raw ×10 Wh in this fork. One physical tracker remains enabled.
- 0x0330–0x0335: reference interprets whole degrees; the table only labels Celsius and leaves the type unspecified. Target-specific provisional /10 remains pending independent validation.
- 0x0190–0x01A2: on-demand BMS raw reads only. 0x7FFF must not become a valid SOC/voltage. Capacity 0x0198/0199 is documented as 10 mAh/count; whether it is per-pack or aggregated remains unverified.
- No calibration, factory or writable inverter controls were imported.
