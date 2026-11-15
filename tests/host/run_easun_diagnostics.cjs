'use strict';
const h = require('./support/native.cjs').harness('easun-diagnostics');
try {
  h.run(h.compile('tests/host/test_easun_diagnostics.cpp', [], {mocked: false}));
} finally { h.cleanup(); }
