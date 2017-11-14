#include "vtinfo.hpp"

#include <exception>
#include <iostream>
#include <utility>

#include "vtzero/vector_tile.hpp"

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

    vtzero::vector_tile tile{node::Buffer::Data(buffer), node::Buffer::Length(buffer)};

    // preparing output
    v8::Local<v8::Object> out = Nan::New<v8::Object>();
    v8::Local<v8::Array> layers = Nan::New<v8::Array>();
    std::size_t layers_size = 0;

    try {
        while (auto layer = tile.next_layer()) {
            v8::Local<v8::Object> layer_obj = Nan::New<v8::Object>();

            std::uint64_t unknown_feature_count = 0;
            std::uint64_t point_feature_count = 0;
            std::uint64_t line_feature_count = 0;
            std::uint64_t polygon_feature_count = 0;
            std::uint64_t invalid_feature_count = 0;

            while (const auto feature = layer.next_feature()) {
                switch (feature.geometry_type()) {
                    case vtzero::GeomType::UNKNOWN:
                        ++unknown_feature_count;
                        break;
                    case vtzero::GeomType::POINT:
                        ++point_feature_count;
                        break;
                    case vtzero::GeomType::LINESTRING:
                        ++line_feature_count;
                        break;
                    case vtzero::GeomType::POLYGON:
                        ++polygon_feature_count;
                        break;
                }
            }

            // keys array
            std::size_t idx = 0;
            v8::Local<v8::Array> keys = Nan::New<v8::Array>(layer.key_table().size());

            for (const auto& key : layer.key_table()) {
                keys->Set(idx++, Nan::New<v8::String>(key.data(), key.size()).ToLocalChecked());
            }

            // features count
            const std::uint64_t features_count = unknown_feature_count + point_feature_count + line_feature_count + polygon_feature_count + invalid_feature_count;

            layer_obj->Set(Nan::New("name").ToLocalChecked(), Nan::New<v8::String>(layer.name().data(), layer.name().size()).ToLocalChecked());
            layer_obj->Set(Nan::New("version").ToLocalChecked(), Nan::New<v8::Number>(layer.version()));
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
