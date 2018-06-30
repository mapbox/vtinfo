#include "vtinfo.hpp"

#include <exception>
#include <iostream>
#include <utility>
#include <protozero/pbf_reader.hpp>
#include <protozero/pbf_writer.hpp>

/**
 * Create an array of JSON objects for each layer in the vector tile buffer to
 * view its attributes and gather general information about the geometries
 *
 * @name info
 * @param {Buffer} buffer - Vector Tile PBF
 * @returns {Object} layers - information about each layer, represented as an array of layers
 * @example
 * var fs = require('fs');
 * var vtinfo = require('vtinfo').info;
 * var buffer = require('/path/to/tile.mvt');
 * var info = vtinfo(buffer);
 * console.log(info); // =>
 *
 */
NAN_METHOD(info)
{

    v8::Local<v8::Object> buffer = info[0]->ToObject();
    if (buffer->IsNull() || buffer->IsUndefined() || !node::Buffer::HasInstance(buffer)) {
        Nan::ThrowTypeError("First argument must be a valid buffer.");
        return;
    }

    // prepare original buffer
    const char *data = node::Buffer::Data(buffer);
    std::size_t dataLength = node::Buffer::Length(buffer);

    // original buffer to be read
    protozero::pbf_reader vt_reader(data, dataLength);

    // preparing output
    v8::Local<v8::Object> out = Nan::New<v8::Object>();
    v8::Local<v8::Array> layers = Nan::New<v8::Array>();
    std::size_t layers_size = 0;

    try {
        // loop through layers
        while (vt_reader.next(3)) {

            auto layer_data = vt_reader.get_data();
            protozero::pbf_reader layer(layer_data);

            v8::Local<v8::Object> layer_obj = Nan::New<v8::Object>();

            // set up keys for layer_obj
            std::string layer_name;
            std::uint32_t layer_version = 1;
            std::uint64_t unknown_feature_count = 0;
            std::uint64_t point_feature_count = 0;
            std::uint64_t line_feature_count = 0;
            std::uint64_t polygon_feature_count = 0;
            std::uint64_t invalid_feature_count = 0;
            std::uint64_t features_count = 0;

            // set up vector for keys
            std::vector<protozero::data_view> keys_vector;

            while (layer.next()) {
                switch(layer.tag()) {
                    case 1: // name
                        layer_name = layer.get_string();
                        break;
                    case 2: // features
                    {
                        // create feature reader
                        auto feature_data = layer.get_data();
                        protozero::pbf_reader feature(feature_data);

                        // loop through feature GeomType
                        while (feature.next(3)) {
                            std::int32_t type = feature.get_enum();
                            if (type == 1) {
                                ++point_feature_count;
                            }
                            else if (type == 2) {
                                ++line_feature_count;
                            }
                            else if (type == 3) {
                                ++polygon_feature_count;
                            }
                            else if (type == 0) {
                                ++unknown_feature_count;
                            }
                            else {
                                // LCOV_EXCL_START
                                ++invalid_feature_count;
                                // LCOV_EXCL_STOP
                            }
                        }
                        break;
                    }
                    case 3: // key
                        keys_vector.push_back(layer.get_view());
                        break;
                    case 4: // value
                        layer.skip();
                        break;
                    case 5: // extent
                        layer.skip();
                        break;
                    case 15: // version
                        layer_version = layer.get_uint32();
                        break;
                    default:
                        throw std::runtime_error("unknown field type " + std::to_string(layer.tag()) + " in layer"); // LCOV_EXCL_LINE
                        break;
                }
            }

            // keys array
            std::size_t idx = 0;
            std::size_t keys_size = keys_vector.size();
            v8::Local<v8::Array> keys = Nan::New<v8::Array>(keys_size);
            for ( auto const& key_view : keys_vector) {
                keys->Set(idx++, Nan::New<v8::String>(key_view.to_string()).ToLocalChecked());
            }

            // features count
            features_count = unknown_feature_count + point_feature_count + line_feature_count + polygon_feature_count + invalid_feature_count;

            layer_obj->Set(Nan::New("name").ToLocalChecked(), Nan::New<v8::String>(layer_name).ToLocalChecked());
            layer_obj->Set(Nan::New("version").ToLocalChecked(), Nan::New<v8::Number>(layer_version));
            layer_obj->Set(Nan::New("features").ToLocalChecked(), Nan::New<v8::Number>(features_count));
            layer_obj->Set(Nan::New("unknown_features").ToLocalChecked(), Nan::New<v8::Number>(unknown_feature_count));
            layer_obj->Set(Nan::New("point_features").ToLocalChecked(), Nan::New<v8::Number>(point_feature_count));
            layer_obj->Set(Nan::New("line_features").ToLocalChecked(), Nan::New<v8::Number>(line_feature_count));
            layer_obj->Set(Nan::New("polygon_features").ToLocalChecked(), Nan::New<v8::Number>(polygon_feature_count));
            layer_obj->Set(Nan::New("invalid_features").ToLocalChecked(), Nan::New<v8::Number>(invalid_feature_count));
            layer_obj->Set(Nan::New("keys").ToLocalChecked(), keys);

            // add layer object to final layers array
            layers->Set(layers_size++, layer_obj);
        }
    } catch (std::exception const& ex) {
      Nan::ThrowTypeError(ex.what());
    }

    out->Set(Nan::New("layers").ToLocalChecked(), layers);
    info.GetReturnValue().Set(out);
    return;
}

extern "C" {
    static void init(v8::Handle<v8::Object> target) {
        Nan::HandleScope scope;
        Nan::SetMethod(target, "info", info);
    }
    #define MAKE_MODULE(_modname) NODE_MODULE( _modname, init);
    MAKE_MODULE(MODULE_NAME);
}
