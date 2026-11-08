# Upstream synchronization

## 2026-11-08 — integration candidate 0.1.0-easun.8-dev

Merged `softwarecrash/Solar2MQTT` main at `c9d9db9b55e86cfc4d796f2adb25b9361fdb2523` into the public EASUN version 7 history. Tag `v0.1.0-easun.7` remains the recovery baseline. The integration source is identified as `0.1.0-easun.8-dev`; no replacement release is published as part of this synchronization.

### Preserved fork behavior

- EASUN SMS/CVTE read-only register driver, addresses, scaling and telemetry mappings from version 7.
- Single-MPPT dashboard, solar display threshold below 10 and the requested experimental load display. Raw watts and VA remain separate; the display substitution does not establish electrical equivalence.
- EASUN Home Assistant current, inverter SOC, DC bus voltage and experimental current descriptors.
- OTA repository `AndyMuc72/SolarMQTT-ESP32-EaSun-SMS-6.5K`, matching board assets, prerelease discovery and exclusion of known problematic `.5`/`.6`.
- Development/local builds can upgrade to the corresponding public release or a later release; `-dev` and `-local` assets are never offered automatically.
- Empty personal Wi-Fi defaults, build guard, image-integrity checks, private-file exclusion and local debug download. No external device-report submission is restored.
- Pinned tested library versions and the version-7 partition layout.

### Upstream changes adopted

- OTA release reader, worker synchronization, single firmware-writer ownership for manual and automatic uploads, download timeout/error handling and orderly restart with persistent-energy flush.
- Read-only PI query scheduling, QMD model fallback and QPIGS/QPIGS2 compatibility fixes.
- Current upstream English UI and Home Assistant labels; upstream removed the old hardcoded language layer.
- Upstream host regressions and board-build workflow, extended to execute the fork OTA-selection regression and dashboard/default checks.

### Validation

- [GitHub host/board checks for the tested runtime commit](https://github.com/AndyMuc72/SolarMQTT-ESP32-EaSun-SMS-6.5K/actions/runs/42475187877): successful Windows, Linux and macOS host suites, plus builds for Wemos D1 mini ESP32, ESP32-C3 SuperMini, ESP32-S3 SuperMini, Waveshare S3 Ethernet and M5Stack ATOM Lite. Linux also runs the extended OTA/upload/PI regressions with a 32-bit ABI and allocation-failure injection.
- Local Wemos firmware build and image checks passed. Both local and downloaded CI images have valid ESP checksums/SHA256, matching USB/OTA apps, a fitting app partition and empty NVS. Public source and candidate images were scanned for the known private credentials; none were found.
- Dashboard: 5,018 assertions plus PV compatibility and actual OTA-page UI checks passed. Local C++ tests passed for OTA/restart, manual-upload ownership/failure handling, five portable host groups, read-only PI scheduling, QMD model fallback and QPIGS compatibility. OTA coverage includes all 14,658 truncated variants of the release fixture. The local Clang test adapter suppresses the existing intentional bool-switch warning; CI GCC tests retain their standard settings.
- Physical local ESP32: installed the GitHub-built OTA image (`sha256:6df67d2a1249ec306744083e270f00d9f1f3224abb218dc9d2e7236b139f5695`), reconnected to its saved Wi-Fi, served matching application/firmware-page assets, passed twelve HTTP health requests, increased uptime from 148 to 213 seconds and retained 159,840 bytes free heap. Simulation stayed disabled.
- USB observation: 180 seconds, exactly one OTA restart, expected new firmware banner, energy backlog ready, no panic/brownout/watchdog/assertion marker. The remote ESP32 was not changed.
- **Live GitHub metadata limitation:** four device update checks returned HTTP 504. A PC comparison using HTTP/1.0 and the same endpoint returned HTTP 503. The device reported these errors and remained reachable; no firmware download/write was started. A successful real-network metadata check is therefore **not claimed** for this integration build. Synthetic production-code tests for successful parsing/selection and failure/retry handling passed.
- Physical inverter/BMS/MQTT measurements and the remote board's original failure remain unverified. Other board variants were compiled, not physically tested. A three-minute observation is not a long-duration stability test.

### Future updates

Start a branch from the tested fork main, fetch upstream main, merge it without force-pushing, resolve conflicts while retaining the policies above, and run host/build/device checks. Never stage private recovery images, flash backups, configuration or diagnostic logs. Public releases must continue to have empty personal Wi-Fi defaults.
