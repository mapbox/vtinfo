# info

Create an array of JSON objects for each layer in the vector tile buffer to
view its attributes and gather general information about the geometries

**Parameters**

-   `buffer` **[Buffer](https://nodejs.org/api/buffer.html)** Vector Tile PBF

**Examples**

```javascript
var fs = require('fs');
var vtinfo = require('vtinfo').info;
var buffer = require('/path/to/tile.mvt');
var info = vtinfo(buffer);
console.log(info); // =>
```

Returns **[Object](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object)** layers - information about each layer, represented as an array of layers
