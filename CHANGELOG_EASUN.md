# EASUN SMS fork release notes

## 0.1.0-easun.2

- Add a downloadable OTA image (`*.bin.ota`) and merged serial-flash image to the tagged GitHub release.
- Point the firmware update check at the `AndyMuc72/SolarMQTT-ESP32-EaSun-SMS-6.5K` fork so devices check this project's releases.
- Decode the six temperature registers as signed tenths of a degree provisionally; the scale still needs confirmation against this exact inverter.
- Build/test status: GitHub Actions firmware compilation must pass for the release asset to be published. Hardware telemetry is not yet fully validated.

## 0.1.0-easun.1

- Add read-only CVTE telemetry for battery SOC and charge current, grid and output current, PV total and PV1 measurements and energy counters, operating/charging states, raw alarms/fault codes, fan and six temperatures.
- Respect the single-MPPT hardware label by not polling PV2 registers `0x0280–0x028C`.
- Add high-word-first 32-bit decoding for PV current, power, and Wh energy registers.
- Add an ESP32 Wemos D1 mini PlatformIO build on pushes and pull requests. The release workflow attaches the firmware binaries after the build succeeds.
- Keep all inverter communication read-only; no Modbus write calls are used by the EASUN telemetry map.

### Validation status

The GitHub Actions result attached to this tag is the firmware compilation status. Table-derived readings still need to be compared with the display of the exact iSolar-SMS-6.5KP-Wifi unit. Only the earlier voltage/frequency/power/battery fields have model-specific empirical support, and even those public scans are from the W-E variant.
