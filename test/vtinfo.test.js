var test = require('tape');
var vtinfo = require('../lib/index.js');
var fs = require('fs');

var buffer = fs.readFileSync(__dirname + '/fixtures/25329.vector.pbf');

test('reads the buffer as expected', function(t) {
  var info = vtinfo(buffer);
  t.equal(info.layers.length, 7, 'expected number of layers');
  t.equal(info.layers[0].name, 'landuse', 'layer names converted properly');
  t.equal(info.layers[0].version, 2, 'version converted properly');
  t.equal(info.layers[0].polygon_features, 5, 'geometry counts stored properly');
  t.equal(info.layers[0].features, 5, 'total feature count');
  t.equal(info.layers[0].keys[0], 'class', 'expected key value in array');
  t.end();
});

test('fails when a non-buffer is passed as an argument', function(t) {
  try {
    var info = vtinfo('not_a_buffer');
  } catch (err) {
    t.ok(err, 'expected an error');
    t.ok(err.message.indexOf('First argument must be a valid buffer') > -1, 'expected error message');
  }
  t.end();
});

test('fails with an invalid vector tile buffer', function(t) {
  try {
    var fakebuffer = new Buffer('waka');
    var info = vtinfo(fakebuffer);
  } catch (err) {
    t.ok(err, 'expected an error');
    t.ok(err.message.indexOf('unknown pbf field type exception') > -1, 'expected error message');
  }
  t.end();
});