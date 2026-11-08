# Upstream synchronization

## 2026-11-08 — integration candidate 0.1.0-easun.8-dev

Merged `softwarecrash/Solar2MQTT` main at `c9d9db9b55e86cfc4d796f2adb25b9361fdb2523` into the public EASUN version 7 history. Tag `v0.1.0-easun.7` remains the recovery baseline. This integration branch is not a replacement release until its tests are recorded below.

### Preserved fork behavior

- EASUN SMS/CVTE read-only register driver, addresses, scaling and telemetry mappings from version 7.
- Single-MPPT dashboard, solar display threshold below 10 and the requested experimental load display. Raw watts and VA remain separate; the display substitution does not establish electrical equivalence.
- EASUN Home Assistant current, inverter SOC, DC bus voltage and experimental current descriptors.
- OTA repository `AndyMuc72/SolarMQTT-ESP32-EaSun-SMS-6.5K`, matching board assets, prerelease discovery and exclusion of known problematic `.5`/`.6`.
- Empty personal Wi-Fi defaults, build guard, image-integrity checks, private-file exclusion and local debug download. No external device-report submission is restored.
- Pinned tested library versions and the version-7 partition layout.

### Upstream changes adopted

- OTA release reader, worker synchronization, single firmware-writer ownership for manual and automatic uploads, download timeout/error handling and orderly restart with persistent-energy flush.
- Read-only PI query scheduling, QMD model fallback and QPIGS/QPIGS2 compatibility fixes.
- Current upstream English UI and Home Assistant labels; upstream removed the old hardcoded language layer.
- Upstream host regressions and board-build workflow, extended to execute the fork OTA-selection regression and dashboard/default checks.

### Validation

In progress. Record actual build, host-test and physical ESP32 results before merging. Physical inverter/BMS/MQTT measurements and the remote board's original failure remain outside these offline tests.

### Future updates

Start a branch from the tested fork main, fetch upstream main, merge it without force-pushing, resolve conflicts while retaining the policies above, and run host/build/device checks. Never stage private recovery images, flash backups, configuration or diagnostic logs. Public releases must continue to have empty personal Wi-Fi defaults.
