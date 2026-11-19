'use strict';
const h = require('./support/native.cjs').harness('easun-mqtt');
try { h.run(h.compile('tests/host/test_easun_mqtt.cpp', [], {mocked: false})); }
finally { h.cleanup(); }
