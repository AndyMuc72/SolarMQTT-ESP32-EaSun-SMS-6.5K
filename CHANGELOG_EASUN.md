# EASUN SMS fork release notes

## 0.1.0-easun.10

- Apply the existing below-10-V / below-10-W PV display policy to outgoing EASUN MQTT snapshots, consistently for flat topics and JSON. Zero associated PV current only when its voltage or power is below the respective threshold. Preserve valid small daytime currents, raw state and energy counters.
- Add bounded read-only BMS, thermal and comparison snapshots, plus experimental output-block references. Keep reserved 0x021B diagnostic-only.
- Correct the battery read-cache range to include the SOC aliases. Keep established solar, active-power, apparent-power and provisional temperature mappings.
- Record CVTE comparison and software/serial test scopes. Keep private recovery credentials out of public builds.


## 0.1.0-easun.9

- Correct the dashboard: active output power is shown in W; apparent output power has its own VA badge. VA is never used as a fallback for watts, including in the load overview.
- Keep missing active power unavailable rather than inventing a watt value from VA. Preserve genuine zero-watt readings.
- Include bounded, read-only EASUN register diagnostics and protocol-aware Register Watch from the `.9-dev` test build.
- Document the 2026-11-14 display comparisons; output current/apparent-power accuracy and BMS SOC remain unverified. No register or scaling changes.
- Preserve empty public Wi-Fi defaults, existing partition layout and the fork OTA index.

## 0.1.0-easun.8

- Restore fork OTA discovery using a compact, automatically published release index; retry transient index errors and retain API fallback.
- Keep actual failed checks visible instead of reporting false up-to-date status. Retain board matching, blocked `.5`/`.6` and one-click installation.
- Include current upstream synchronization; preserve version-7 EASUN register mappings, partition layout and empty personal Wi-Fi defaults. See [OTA validation](OTA_VALIDATION.md).

## 0.1.0-easun.8-dev (upstream integration; not a published release)

- Merge current upstream main while retaining version-7 EASUN registers, scaling, dashboard behavior, fork OTA repository and empty personal Wi-Fi defaults.
- Adopt upstream OTA writer ownership, restart handling and PI protocol/model fixes. Keep fork prerelease selection and blocked `.5`/`.6`; allow development builds to upgrade to public releases.
- Preserve the version-7 tag and release files. See [upstream synchronization notes](UPSTREAM_SYNC.md) for actual host/build/ESP32 results and the live GitHub HTTP 503/504 limitation.

## 0.1.0-easun.7

- Rebase repairs on working `.4`; fix `.6` dashboard syntax and GitHub HTTPS JSON truncation/task-handle race.
- Add mandatory syntax/dashboard CI checks, empty Wi-Fi-default guard, release image integrity checks and SHA256 files.
- Keep raw telemetry unchanged; apply requested experimental EASUN load-display substitution and solar display threshold below 10.
- Publish only ordinary firmware without personal Wi-Fi configuration. See [full test status and installation notes](RELEASE_NOTES.md).

## 0.1.0-easun.4

- Use the target-validated `0x021C` value as `AC_Out_Watt`; keep `0x0085` visible only as a reference because it read 0 W while the WiFi/FW8 target reported 128 W at low load.
- Use `0x0220` (2.1%) as the target's output load percentage; retain `0x005D` (21) as a diagnostic reference because it is inconsistent with the measured 128 W on a 6.5 kW inverter.
- Publish `0x0156` as canonical signed `Battery_Current` and signed net `Battery_Load`, retaining `Battery_Average_Current` for compatibility. Treat `0x0157` as experimental until its semantics are verified.
- Rename the 0x0154 field to `DC_Bus_Voltage`; expose the inverter-reported SOC estimate separately and document that it does not match the Pylontech BMS SOC.
- Build/test status: pending this release's GitHub Actions Wemos D1 mini ESP32 build. This verifies compilation only; hardware readings still need fresh live comparison.

## 0.1.0-easun.3

- Add the experimental temperature candidates at `0x00A4–0x00A6` reported in syssi/esphome-smg-ii PR #163. Publish the raw unsigned values under explicit experimental names; component assignments and scaling are not confirmed.
- Keep the generic `0x0330–0x0335` temperature fields at the provisional ×0.1 scale because that matches the observed WiFi-labelled unit values, while documenting that the supplied CVTE table specifies whole °C.
- Build/test status: GitHub Actions compilation is the release gate; exact WiFi-model temperature readings still require comparison with the app or an independent probe.

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
