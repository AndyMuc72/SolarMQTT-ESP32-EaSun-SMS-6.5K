# EASUN read-only register diagnostics

Command Sender accepts these commands only with `MODBUS_EASUN_SMS` selected:

* `easun read 0x0152` — read one register.
* `easun read 338 6` — decimal address, six registers.
* `easun read 0x0219 8` — compare AC output candidates.
* `easun watch` — take a fresh snapshot of 0x0100–0102, 0x0152–0157 and 0x0219–0220.

Register Watch now chooses `easun watch` for EASUN; PowMr retains its existing command.
EASUN Watch is a fresh snapshot, not the PowMr baseline/difference feature.
Save snapshots at stable low and high loads together with independently measured W,
VA, current and battery/BMS SOC. The candidate SRNE addresses are not confirmed EASUN mappings.

Each row gives the zero-based register address (hex and decimal), raw hexadecimal,
unsigned 16-bit and signed 16-bit values. No scale or unit is inferred. In particular,
32767 is displayed as raw data, not asserted to be a valid SOC or temperature.

Only FC03 is available. Each read is limited to 1–15 words; malformed numbers,
overflow, extra arguments and ranges beyond 0xFFFF are rejected before transport.
There is no write command or arbitrary function-code/frame interface.
Reads run in the existing serial command consumer between regular polling operations,
with one attempt and a 400 ms timeout per block. Watch has three blocks (up to roughly
1.2 seconds of response timeout plus transport/logging overhead). No background task,
continuous scan, telemetry mutation or persistent configuration is added. Read-cache
invalidation prevents diagnostics from supplying cached samples to telemetry.
Failures include the raw Modbus result code (e.g. 0x02 illegal address, 0xE2 timeout,
0xE3 CRC); failed blocks produce no measurement rows. Other blocks can still succeed.

Validation: `node tests/host/run_easun_diagnostics.cjs`, `node tests/test_ui.cjs`,
public-default and firmware-image checks. A live EASUN register validation requires
the inverter hardware; host tests do not establish the correctness of candidate mappings.
