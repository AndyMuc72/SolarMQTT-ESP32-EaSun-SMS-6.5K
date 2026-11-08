import base64
import copy
import importlib.util
import json
from pathlib import Path
import unittest
from unittest.mock import patch
from urllib.error import HTTPError

spec = importlib.util.spec_from_file_location('index', Path(__file__).resolve().parents[1] / 'tools/update_ota_manifest.py')
index = importlib.util.module_from_spec(spec)
spec.loader.exec_module(index)


def release(number):
    tag = f'v0.1.0-easun.{number}'
    name = f'Solar2MQTT_wemos_d1_mini32_V0.1.0-easun.{number}.bin.ota'
    return {'tag_name': tag, 'prerelease': True, 'body': 'must be excluded', 'assets': [
        {'name': name, 'browser_download_url': f'https://github.com/{index.REPOSITORY}/releases/download/{tag}/{name}',
         'size': 1600000, 'digest': 'sha256:' + 'a' * 64}]}


class IndexTests(unittest.TestCase):
    def test_compact_and_ordered(self):
        result = index.build_manifest(release(8), [release(7), release(9)])
        self.assertEqual([r['tag_name'] for r in result], ['v0.1.0-easun.9', 'v0.1.0-easun.8', 'v0.1.0-easun.7'])
        self.assertNotIn('body', json.dumps(result))
        self.assertEqual(result[1]['assets'][0]['digest'], 'sha256:' + 'a' * 64)

    def test_invalid_releases(self):
        for value in (release(5), release(6), dict(release(8), draft=True), dict(release(8), tag_name='v0.1.0-easun.8-dev')):
            with self.assertRaises(ValueError): index.build_manifest(value, [])

    def test_invalid_assets(self):
        for field, value in (('size', 0), ('browser_download_url', 'https://example.invalid/firmware'), ('name', 'firmware.bin')):
            data = release(8); data['assets'][0][field] = value
            with self.assertRaises(ValueError): index.sanitize_release(data)

    def test_old_invalid_entries_and_limit(self):
        previous = [release(i) for i in range(7, 24)] + [None, {'tag_name': 'bad'}]
        result = index.build_manifest(release(24), previous)
        self.assertEqual(len(result), 10)
        self.assertEqual(result[0]['tag_name'], 'v0.1.0-easun.24')

    def test_conflict_preserves_concurrent_newer_release(self):
        calls = []
        reads = [0]
        def api(path, method='GET', body=None):
            if '/releases/tags/' in path: return release(8)
            if method == 'GET':
                reads[0] += 1
                prior = [release(7)] if reads[0] == 1 else [release(9), release(7)]
                return {'sha': str(reads[0]), 'content': base64.b64encode(json.dumps(prior).encode()).decode()}
            calls.append(copy.deepcopy(body))
            if len(calls) == 1: raise HTTPError(path, 409, 'conflict', {}, None)
            return {}
        with patch.object(index, 'api', api), patch.object(index.time, 'sleep'):
            index.publish(index.REPOSITORY, 'v0.1.0-easun.8')
        result = json.loads(base64.b64decode(calls[-1]['content']))
        self.assertEqual(result[0]['tag_name'], 'v0.1.0-easun.9')
        self.assertEqual(calls[-1]['sha'], '2')
        self.assertEqual(calls[-1]['branch'], 'main')

    def test_repository_restriction(self):
        with patch.object(index, 'api') as api:
            with self.assertRaises(ValueError): index.publish('another/repo', 'v0.1.0-easun.8')
            api.assert_not_called()


if __name__ == '__main__': unittest.main()
