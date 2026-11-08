"""Check packaged Wemos images and emit SHA256 sidecars before uploading."""
from pathlib import Path
import hashlib
import struct

root = Path(__file__).resolve().parents[1]
images = list((root / '.firmware').glob('*.bin'))
if len(images) != 1:
    raise SystemExit('Expected exactly one merged release image')
merged = images[0]
ota = Path(str(merged) + '.ota')
data = merged.read_bytes()
app = ota.read_bytes()
assert len(data) == 0x400000, 'Expected 4 MB merged image'
assert data[0x9000:0xe000] == b'\xff' * 0x5000, 'Public NVS must be empty'
assert data[0x10000:0x10000+len(app)] == app, 'OTA app must match merged app'
assert len(app) <= 0x1c0000, 'App exceeds OTA partition'
assert data[0x8000:0x8002] == b'\xaa\x50', 'Missing partition table'
def verify_esp_image(blob):
    assert blob[0] == 0xe9, 'Invalid ESP image magic'
    offset, checksum = 24, 0xef
    for _ in range(blob[1]):
        _, size = struct.unpack_from('<II', blob, offset)
        offset += 8
        segment = blob[offset:offset+size]
        assert len(segment) == size
        for value in segment:
            checksum ^= value
        offset += size
    end = (offset + 16) & ~15
    assert blob[end-1] == checksum, 'ESP segment checksum mismatch'
    if blob[23]:
        assert blob[end:end+32] == hashlib.sha256(blob[:end]).digest(), 'ESP SHA256 mismatch'
verify_esp_image(app)
verify_esp_image(data[0x1000:])
for image in (merged, ota):
    digest = hashlib.sha256(image.read_bytes()).hexdigest()
    Path(str(image)+'.sha256').write_text(digest+'  '+image.name+'\n', encoding='ascii')
    print('PASS:', image.name, image.stat().st_size, 'bytes, SHA256', digest)
print('PASS: empty NVS, matching OTA/USB app, partition size and ESP integrity')
