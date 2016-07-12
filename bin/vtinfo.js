#!/usr/bin/env node

'use strict';

var fs = require('fs');
var path = require('path');
var vtinfo = require(path.resolve(__dirname, '..', 'lib', 'index.js'));

var file = process.argv[2];
var buffer;
try {
  buffer = fs.readFileSync(file);
} catch (err) {
  console.error(err);
  process.exit(1);
}

console.log(vtinfo(buffer));
process.exit(0);