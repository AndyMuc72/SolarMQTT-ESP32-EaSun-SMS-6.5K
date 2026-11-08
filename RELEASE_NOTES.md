# 0.1.0-easun.8 — reliable fork OTA discovery

The update checker now reads a small release index from this repository instead of relying on the GitHub release-list API that returned HTTP 503/504 during real-device tests. Published EASUN releases, including prereleases, are downloaded from this repository's board-specific GitHub release assets.

## Behavior

- A successful check reports **Up to date** when no newer eligible release exists. A newer release enables the update button; clicking it downloads the matching `*.bin.ota`, writes it and restarts the device.
- `ota/releases.json` is refreshed automatically after release assets exist, for both workflow-created and manually published releases. No GitHub token is stored on the ESP32.
- Transient index HTTP 502/503/504 or transport failures receive one bounded retry, followed by an API fallback if needed. Failed checks remain errors and are never disguised as up-to-date results.
- Drafts, `.5`/`.6`, development/local releases and wrong-board assets remain excluded. Local/development builds can upgrade to their corresponding ordinary release.
- Includes upstream synchronization and OTA writer/restart safeguards. EASUN register addresses, measurement scaling and version-7 partition layout remain unchanged.

## Installation and privacy

- Public images contain **no personal Wi-Fi defaults or saved configuration**. Private recovery files remain private.
- USB: flash `Solar2MQTT_wemos_d1_mini32_V0.1.0-easun.8.bin` at **0x0**. This merged image clears configuration; set up Wi-Fi afterward.
- Manual OTA: upload `Solar2MQTT_wemos_d1_mini32_V0.1.0-easun.8.bin.ota` through `/firmware`; existing configuration is preserved. App-only OTA does not migrate partition tables.
- Firmware still using the API-only checker may need this repair installed manually once if its GitHub API requests continue to fail. Subsequent releases use the compact index and the update button.
- SHA256 sidecars are supplied. Only use the correct board variant.

## Test status

Publication requires the Wemos build, image-integrity/empty-NVS checks, dashboard tests and release-index publisher tests. Production OTA tests cover parsing, index retries/fallback, invalid/truncated JSON, board matching, blocked releases and write failures. Real-device check/update results are recorded in `OTA_VALIDATION.md`; full inverter/BMS measurements and long-duration stability remain unverified.
