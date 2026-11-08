"""Reject personal Wi-Fi defaults in all ordinary builds."""
from pathlib import Path
import re

root = Path(__file__).resolve().parents[1]
schema = (root / 'src/core/SettingsPrefs.schema.h').read_text(encoding='utf-8')
for key in ('wifiSsid0', 'wifiPassword0', 'wifiSsid1', 'wifiPassword1'):
    match = re.search(r'"' + key + r'",\s*' + key + r',\s*"([^"]*)"', schema)
    if not match or match.group(1):
        raise SystemExit('Public build rejected: Wi-Fi defaults must be empty (' + key + ')')
print('PASS: all public Wi-Fi defaults are empty')
