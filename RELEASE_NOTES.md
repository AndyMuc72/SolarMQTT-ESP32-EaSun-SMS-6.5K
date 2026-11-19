# 0.1.0-easun.10 — consistent solar MQTT zero values and read-only CVTE diagnostics

MQTT previously published residual PV values such as 1.7 V / 0.58 A / 1 W while the dashboard displayed zero. EASUN outgoing MQTT snapshots now apply the same below-10-V / below-10-W thresholds. Associated current is zeroed when voltage or power is below its threshold. Small real daytime currents remain valid. Flat LiveData topics and the Data JSON use the same normalized snapshot; original state/diagnostics and energy counters are preserved.

## Included changes

- `easun compare`, `easun thermal`, `easun bms`: bounded on-demand FC03 raw snapshots through the existing serial queue. No writes or extra polling tasks.
- Experimental `AC_Out_Small_Current_021B_Experimental` plus output voltage/frequency references. CVTE marks 0x021B reserved; it does not replace the established current.
- Battery read-cache range includes its SOC aliases.
- Keep established solar mappings, separate active W/apparent VA, and existing provisional temperature scaling. Unloaded VA/A readings remain unvalidated; they are not relabelled as self-consumption watts or offset-corrected.

## Installation

- Wemos D1 mini ESP32 only: use `Solar2MQTT_wemos_d1_mini32_V0.1.0-easun.10.bin.ota` in the firmware web page. App-only OTA preserves the existing partition layout and settings.
- USB recovery: `Solar2MQTT_wemos_d1_mini32_V0.1.0-easun.10.bin` is the complete 4 MB image for address 0x0 and clears configuration. Do not upload it as OTA.
- If currently running a `-cvte-test` build, install this public OTA file manually once: those test builds do not normalize their version suffix for automatic comparison. Ordinary public versions use the normal one-click fork update path.
- Public Wi-Fi defaults are empty. No private recovery image, stored credentials or serial logs are included.
- Fork update discovery uses this release's board-specific OTA asset and compact index; defective .5/.6 remain blocked.

## Test status

Functional test candidate `.10-cvte-test.2`: native MQTT regression, 5,029-assertion UI suite, image integrity checks, and 180-second COM5 boot test passed. See [validation details](Protocol/EASUN_10_VALIDATION.md). CI results and exact public-image test status are attached to this release. Broker delivery, inverter/BMS measurements and long-term operation remain unverified on hardware.
