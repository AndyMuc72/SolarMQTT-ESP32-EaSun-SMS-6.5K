'use strict';
const assert = require('node:assert/strict');
const fs = require('node:fs');
const vm = require('node:vm');
const source = fs.readFileSync('src/webUI/app.js', 'utf8');
const start = source.indexOf('  bindClick("powmrWatchBtn",');
const end = source.indexOf('  bindClick("rebootBtn",', start);
assert(start >= 0 && end > start);
async function main() {
  for (const protocol of ['MODBUS_EASUN_SMS', 'MODBUS_POWMR']) {
    for (const failure of ['none', 'total', 'partial']) {
      const failed = failure !== 'none';
      const calls = [], notices = [], texts = {};
      let click;
      const answer = failure === 'partial' ? 'EASUN_WATCH raw snapshot\nERROR: EASUN FC03 timeout' : failed ? 'ERROR: EASUN FC03 timeout' : protocol === 'MODBUS_EASUN_SMS' ? 'EASUN_WATCH raw snapshot' : 'POWMR_WATCH baseline';
      const ctx = vm.createContext({
        state: {activeProtocol: protocol}, URLSearchParams, Date,
        bindClick(id, handler) { assert.equal(id, 'powmrWatchBtn'); click = handler; },
        async fetchJson(url, options) { calls.push([url, options.method, options.body.get('command')]); },
        async waitForCommandAnswer() { return {RawData: {CommandAnswer: answer}}; },
        setText(id, text) { texts[id] = text; }, byId() { return null; },
        showNotice(text, error = false) { notices.push([text, error]); },
      });
      vm.runInContext(source.slice(start, end), ctx);
      await click();
      assert.deepEqual(calls, [['/api/command', 'POST', protocol === 'MODBUS_EASUN_SMS' ? 'easun watch' : 'powmr watch']]);
      assert.equal(texts.commandAnswer, answer);
      assert.equal(notices.length, 1);
      assert.equal(notices[0][1], failed);
    }
  }
  console.log('PASS: real Register Watch handler routes EASUN/PowMr, displays snapshots and reports failures');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
