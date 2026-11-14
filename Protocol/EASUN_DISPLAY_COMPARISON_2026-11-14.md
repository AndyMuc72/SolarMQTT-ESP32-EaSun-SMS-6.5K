# Device-display comparison, 2026-11-14

User-supplied display photos and nearby `.9-dev` telemetry, EASUN SMS-6.5KP-WiFi,
inverter firmware 8. These are display comparisons, not calibrated measurements.
Photos and JSON were not captured atomically; no external power meter was supplied.
The connected output loads were uncertain, so this is not an established no-load test.

| Quantity | Device display | Nearby telemetry | Assessment |
|---|---|---|---|
| Battery voltage | 49.6 V | 49.6 V | Agreement at this operating point |
| Output frequency | 50.0 Hz | 50 Hz | Agreement |
| PV daily energy | about 8.6 kWh | 8640 Wh | Consistent with display rounding |
| Active output power (second photo, explicit W) | 167 W | 172 W at reference 0x0085; 176 W at 0x021C | Both close, neither uniquely established; timing/load changes possible |
| First photo's output indication | 166, unit appears VA | 166 at 0x0085; 168 at 0x021C; 400 at 0x021D | Unit interpretation needs another comparison; 0x021D is disputed |
| Output apparent power (second sample) | Not captured on this page | 398 VA | Not confirmed |
| Output current | Not captured | 1.74 A | Not confirmed |
| Battery discharge current | 0.0 A | 0 A | Agreement in reporting, not proof of actual zero current |

Despite Battery Mode, zero grid/PV power and nonzero output power, reported battery
current was zero. Keep the limitation visible; do not infer a corrected current or
trust calculated battery energy as independently validated at this point.
The inverter SOC (61% in these samples) is not established as Pylontech BMS SOC.

Release `.9` fixes only the unit/presentation issue: `AC_Out_Watt` stays watts and
`AC_Out_VA` stays VA. It does not subtract a current offset, switch registers, change
scaling, or reinterpret VA as W. Raw JSON/MQTT readings remain unchanged.

Next evidence: repeat display A/VA/W pages and register snapshots at stable small and
larger known loads, preferably accompanied by an external meter's W, A and VA/PF.
