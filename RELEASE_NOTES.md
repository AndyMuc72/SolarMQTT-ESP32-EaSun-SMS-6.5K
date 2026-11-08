# 0.1.0-easun.7

Public firmware for **Wemos D1 mini ESP32 / EASUN SMS-6.5KP WiFi**. Based on the working `.4` source. No personal Wi-Fi defaults or saved configuration are included. This is the default for future public releases too.

## Fixes

- Fix the `.6` dashboard JavaScript syntax error. Every build now checks JavaScript syntax and runs dashboard regression tests in CI.
- Fix truncated GitHub release JSON during HTTPS transport gaps (`IncompleteInput`) using a buffered reader with an idle timeout. Fix the OTA worker task-handle race. Skip known problematic `.5` and `.6` updates.
- Display solar voltage/power below 10 as zero without modifying raw telemetry.
- For this EASUN profile, use the VA register as the requested experimental dashboard load-power source. Raw W/VA telemetry remains separate. **This display substitution does not establish that VA equals real watts**; verify against a power meter. No PV register scaling or solar power calibration was changed.
- Retain `.4` telemetry mappings; exclude `.5` experimental BMS changes. No confirmed cause of the original board's alleged boot loop was established. `.6` did boot on the second test board; its confirmed defects were the UI and update checker.
- Remove the optional external device-report submission. Local debug download remains available.
- Pin tested library versions; reject populated Wi-Fi defaults before building; validate image integrity and empty NVS before publishing.

## Test status and limits

The repair candidate passed 6 OTA reader self-tests on a physical ESP32, 3 real GitHub update checks, repeated HTTP requests and a 120-second serial boot observation without panic, watchdog or brownout. Dashboard tests cover 5,018 assertions including raw-data preservation, plus 6 PV compatibility scenarios. The final public `.7` firmware also passed a local build, ESP image checksum/SHA validation, empty-NVS and private-credential scans. It was installed by OTA on the test ESP32, reconnected to its existing Wi-Fi, served JavaScript identical to the public source and passed three more real GitHub update checks and 12 HTTP health checks. No personal Wi-Fi defaults were included in that firmware.

**GitHub Actions status:** source/default/dashboard checks passed; the compiler build was blocked by repeated HTTP 504 responses while downloading official ESP32 tooling, before compilation. [Failed CI attempts and original logs](https://github.com/AndyMuc72/SolarMQTT-ESP32-EaSun-SMS-6.5K/actions/runs/42469403771). This prerelease uses locally built artifacts, not successful CI build artifacts. The automatic release workflow still requires successful compilation and image checks. This is not a long-duration or connected-inverter validation. Physical inverter, Pylontech BMS and MQTT measurement validation remain open.

## Installation

- **USB recovery:** flash `Solar2MQTT_wemos_d1_mini32_V0.1.0-easun.7.bin` at **0x0**. This merged image contains an empty NVS region and clears saved device configuration. Configure Wi-Fi through the device's setup access point afterward.
- **Web OTA:** upload `Solar2MQTT_wemos_d1_mini32_V0.1.0-easun.7.bin.ota`. This app-only image preserves existing configuration and does not migrate partition tables. If the board has a different partition layout or cannot boot, use USB recovery instead.
- SHA256 sidecars are attached for both images. These images contain no preconfigured personal Wi-Fi credentials. Do not use private recovery images as release assets.
