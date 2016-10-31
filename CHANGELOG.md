## 0.1.2

- Prettify output to prevent `[Object]` from printing in your shell [#15](https://github.com/mapbox/vtinfo/pull/15)

## 0.1.1

- Optimize protozero usage by replacing `protozero::pbf_reader` with `protozero::data_view`, which provides pointers to the keys instead.

## 0.1.0

- Add `keys` value to the response for each layer. `keys` in array of strings that represent all the existing keys in the properties of the features of the vector tile.

## 0.0.5

- Update protozero dependency and `require` instead of pointing to the header files. [#11](https://github.com/mapbox/vtinfo/pull/11)

## 0.0.4

- use error from Protozero within try/catch

## 0.0.3

- add `layer.features` count of all features

## 0.0.2

- Update CLI error messages
- Begin building binaries on TravisCI

## 0.0.1

- First