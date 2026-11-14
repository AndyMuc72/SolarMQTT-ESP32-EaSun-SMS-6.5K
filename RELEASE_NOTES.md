# 0.1.0-easun.9 — separate watts and VA; EASUN diagnostics

The dashboard previously substituted the apparent-power register (VA) for active output power and labelled the result W. This release corrects both the load summary and AC Out L1: watts come from the existing active-power fields, and apparent power is shown separately with a VA badge. A missing watt measurement is not replaced by VA; a genuine zero-watt measurement remains zero.

## Included diagnostics

Register Watch now sends `easun watch` for MODBUS_EASUN_SMS. Command Sender accepts `easun read 0x0152` or `easun read 0x0219 8`. Reads are FC03 only, 1–15 registers per request, through the existing serial command consumer. Raw output includes hex, unsigned and signed 16-bit values and communication failures. See [diagnostic instructions](Protocol/EASUN_REGISTER_DIAGNOSTICS.md).

## Measurement status

[Display comparisons from 2026-11-14](Protocol/EASUN_DISPLAY_COMPARISON_2026-11-14.md) support battery voltage, output frequency and daily PV energy at the observed points. Displayed output watts are close to both candidate power registers, but timing and load differences prevent choosing a new mapping. Output current, apparent power, low-current battery reporting and authoritative BMS SOC remain unresolved. This release changes no telemetry addresses, scaling or raw JSON/MQTT values. Displaying VA separately does not validate that reading.

## Installation and privacy

- Public images contain no personal Wi-Fi defaults or saved configuration.
- Manual OTA: upload `Solar2MQTT_wemos_d1_mini32_V0.1.0-easun.9.bin.ota` through `/firmware`. The unchanged partition layout preserves existing configuration with app-only OTA.
- USB: flash `Solar2MQTT_wemos_d1_mini32_V0.1.0-easun.9.bin` at `0x0`. This is a complete 4 MB image with empty NVS and clears existing configuration.
- Fork OTA discovery uses this repository's compact release index; `.5` and `.6` remain excluded. An installed `.9-dev` can upgrade to this ordinary `.9` release.
- SHA256 sidecars are supplied. Use the matching Wemos D1 mini ESP32 variant.

## Test status

Release publication follows successful dashboard/W–VA separation and raw-data preservation tests, diagnostic parser and Watch-handler tests, OTA manifest tests, host regressions and the Wemos firmware build/image-integrity/empty-NVS checks. CI links record the actual results. No new physical-device installation or long-duration test is claimed for this build; `.9-dev` diagnostic responses and the display comparisons were supplied by the user.
