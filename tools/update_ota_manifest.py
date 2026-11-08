"""Publish a small public OTA index after GitHub release assets are available."""
import base64
import json
import os
import re
import time
from urllib.error import HTTPError
from urllib.parse import quote
from urllib.request import Request, urlopen

REPOSITORY = 'AndyMuc72/SolarMQTT-ESP32-EaSun-SMS-6.5K'
MANIFEST = 'ota/releases.json'


def sanitize_release(release, repository=REPOSITORY):
    tag = release.get('tag_name', '')
    match = re.fullmatch(r'v?(0\.1\.0-easun\.(\d+))', tag)
    if not match or int(match[2]) in (5, 6) or release.get('draft'):
        raise ValueError('Not an eligible public EASUN release')
    version = match[1]
    prefix = f'https://github.com/{repository}/releases/download/{quote(tag, safe="")}/'
    assets = []
    names = set()
    for asset in release.get('assets', []):
        name = asset.get('name', '')
        if not re.fullmatch(r'Solar2MQTT_[a-z0-9_]+_V' + re.escape(version) + r'\.bin\.ota', name):
            continue
        if asset.get('browser_download_url') != prefix + quote(name, safe=''):
            raise ValueError('Release asset URL does not match this repository/tag')
        if name in names or not isinstance(asset.get('size'), int) or asset['size'] <= 0:
            raise ValueError('Duplicate or empty OTA asset')
        names.add(name)
        item = {key: asset[key] for key in ('name', 'browser_download_url', 'size')}
        digest = asset.get('digest', '')
        if isinstance(digest, str) and re.fullmatch(r'sha256:[0-9a-f]{64}', digest):
            item['digest'] = digest
        assets.append(item)
    if not assets:
        raise ValueError('No complete board-specific OTA release asset')
    return {'tag_name': tag, 'draft': False, 'prerelease': bool(release.get('prerelease')),
            'assets': sorted(assets, key=lambda item: item['name'])}


def build_manifest(release, previous):
    current = sanitize_release(release)
    entries = {}
    for old in previous if isinstance(previous, list) else []:
        try:
            item = sanitize_release(old)
            entries[int(item['tag_name'].split('.')[-1])] = item
        except (ValueError, TypeError, AttributeError):
            continue
    entries[int(current['tag_name'].split('.')[-1])] = current
    return [entries[number] for number in sorted(entries, reverse=True)[:10]]


def api(path, method='GET', body=None):
    token = os.environ['GITHUB_TOKEN']
    data = None if body is None else json.dumps(body).encode('utf-8')
    request = Request('https://api.github.com' + path, data=data, method=method, headers={
        'Authorization': 'Bearer ' + token, 'Accept': 'application/vnd.github+json',
        'X-GitHub-Api-Version': '2022-11-28', 'User-Agent': 'Solar2MQTT-release-index',
        'Content-Type': 'application/json'})
    with urlopen(request, timeout=30) as response:
        return json.load(response)


def publish(repository, tag, branch='main'):
    if repository != REPOSITORY:
        raise ValueError('OTA publication is restricted to the configured fork')
    release = api(f'/repos/{repository}/releases/tags/{quote(tag, safe="")}')
    sanitize_release(release)
    path = f'/repos/{repository}/contents/{MANIFEST}'
    for attempt in range(3):
        try:
            stored = api(path + '?ref=' + quote(branch, safe=''))
        except HTTPError as error:
            if error.code != 404:
                raise
            stored = {}
        previous = json.loads(base64.b64decode(stored['content'])) if stored else []
        content = (json.dumps(build_manifest(release, previous), indent=2) + '\n').encode('utf-8')
        if stored and base64.b64decode(stored['content']) == content:
            print('OTA index already matches published release', tag)
            return
        body = {'message': 'Update OTA index for ' + tag, 'branch': branch,
                'content': base64.b64encode(content).decode('ascii')}
        if stored:
            body['sha'] = stored['sha']
        try:
            api(path, 'PUT', body)
            print('Published compact OTA index for', tag)
            return
        except HTTPError as error:
            if error.code != 409 or attempt == 2:
                raise
            time.sleep(1)
    raise RuntimeError('Could not update OTA index')


if __name__ == '__main__':
    publish(os.environ['GITHUB_REPOSITORY'], os.environ['OTA_RELEASE_TAG'])
