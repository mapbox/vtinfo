# vtinfo

Get general information and stats about a particular vector tile buffer. Meant as a lightweight replacement to Node Mapnik's [`mapnik.VectorTile.info()`](http://mapnik.org/documentation/node-mapnik/3.5/#VectorTile.info).

# Usage

Installation

```
npm install vtinfo
```

Get information as JSON

```javascript
var fs = require('fs');
var vtinfo = require('vtinfo');

// get a vector tile buffer
var buffer = fs.readFileSync('/path/to/tile.vector.pbf');

// get info about the buffer
var info = vtinfo(buffer);
console.log(info);
// { layers: 
//    [ { name: 'landuse',
//        version: 2,
//        unknown_features: 0,
//        point_features: 0,
//        line_features: 0,
//        polygon_features: 5,
//        invalid_features: 0 },
//      { name: 'water',
//        version: 2,
//        unknown_features: 0,
//        point_features: 0,
//        line_features: 0,
//        polygon_features: 3,
//        invalid_features: 0 } 
//   ]
// }
```

Command line

```bash
vtinfo <path to tile>
```

# Develop & Test

Set up your environment by running `make`, which will install dependencies and build binaries.

Run tests with `npm test`.