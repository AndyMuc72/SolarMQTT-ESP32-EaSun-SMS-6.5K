# OTA discovery repair — 2026-11-08

## Observed failure

The GitHub release-list API returned HTTP 503/504 both on the ESP32 and on the PC. A GitHub Raw source file and the actual release-7 OTA asset returned HTTP 200. The previous integration build correctly reported a failed check but could not discover releases during this API failure. Reporting "up to date" after such a failure would have been incorrect.

## Repair

The EASUN checker requests the compact, API-compatible `ota/releases.json` from GitHub Raw, with a cache-busting query and no-cache/identity headers. The release publisher validates exact repository/tag/board asset URLs and updates this file only after a public release has complete OTA assets. An optimistic file-SHA update preserves concurrent releases. The checker retains a bounded retry and API fallback. Firmware download/write ownership and restart safeguards remain in place.

## Validation

- Six publisher unit tests passed: compact output, ordering, blocked/draft/development releases, invalid/empty assets, history limit and preserving a concurrent newer release after a write conflict.
- Production OTA/restart host tests passed, including all 14,658 release-response truncations and new index retry/fallback/no-false-success scenarios.
- Dashboard (5,018 assertions) and OTA-page UI tests passed.
- The seeded release-7 index returned HTTP 200 and was 492 bytes.
- Physical update-cycle validation and release workflow verification are in progress. Record final outcomes here before marking the repair complete.
