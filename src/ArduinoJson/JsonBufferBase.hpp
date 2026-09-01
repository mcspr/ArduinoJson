// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Deserialization/JsonParser.hpp"
#include "Deserialization/DeserializationOptions.hpp"

namespace ArduinoJson {
namespace Internals {

// User-facing parser API. Generally, everything string-like that is declared via StringTraits is supported.
template <typename TImpl>
class JsonBufferBase : public JsonBuffer {
 public:
  // Generalized method to immediately assign JSON to a JsonVariant one of the types:
  // JsonArray&, JsonObject& or const char* (as RawJson); make sure to verify it through ::is<T>() or ::success() before using
  template <typename TData,
    typename TParseString = JsonParserImpl::ParseStringImpl,
    template <typename, typename, typename> class TJsonParser = Internals::JsonParser,
    typename TDeserializationOptions = DefaultDeserializationOptions>
  JsonVariant parse(TData&& json, DeserializationOptions deserializationOptions = makeDefaultDeserializationOptions(TDeserializationOptions())) {
    return Internals::makeParser<TJsonParser, TParseString>(
      impl(), std::forward<TData>(json), deserializationOptions).parseVariant();
  }

  // Parse and populate a JsonArray from a JSON string, returning a reference to it.
  // Returns JsonArray::invalid() on parsing errors or buffer allocation failures.
  template <typename TData,
    typename TParseString = JsonParserImpl::ParseStringImpl,
    template <typename, typename, typename> class TJsonParser = Internals::JsonParser,
    typename TDeserializationOptions = DefaultDeserializationOptions>
  JsonArray& parseArray(TData&& json, DeserializationOptions deserializationOptions = makeDefaultDeserializationOptions(TDeserializationOptions())) {
    return Internals::makeParser<TJsonParser, TParseString>(
      impl(), std::forward<TData>(json), deserializationOptions).parseArray();
  }

  // Parse and populate a JsonObject from a JSON string, returning a reference to it.
  // Returns JsonObject::invalid() on parsing errors or buffer allocation failures.
  template <typename TData,
    typename TParseString = JsonParserImpl::ParseStringImpl,
    template <typename, typename, typename> class TJsonParser = Internals::JsonParser,
    typename TDeserializationOptions = DefaultDeserializationOptions>
  JsonObject& parseObject(TData&& json, DeserializationOptions deserializationOptions = makeDefaultDeserializationOptions(TDeserializationOptions())) {
    return Internals::makeParser<TJsonParser, TParseString>(
      impl(), std::forward<TData>(json), deserializationOptions).parseObject();
  }

  // Simplified version of parseObject(..., {nestingLimit=1}) w/o implicit object allocation
  // CAUTION: only works for flat objects {key1: val1, key2: val2, ...}
  // Returns false on parsing errors and intermediate buffer allocation failures.
  template <typename TData, typename TCallback,
    typename TParseString = JsonParserImpl::ParseStringImpl,
    template <typename, typename, typename> class TJsonParser = Internals::JsonKeyValueParser,
    typename TDeserializationOptions = DefaultDeserializationOptions>
  bool parseKeyValue(TData&& json, TCallback&& callback, DeserializationOptions deserializationOptions = makeDefaultDeserializationOptions(TDeserializationOptions())) {
    return Internals::makeParser<TJsonParser, TParseString>(
      impl(), std::forward<TData>(json), DeserializationOptions(1, deserializationOptions.enableComments, deserializationOptions.skipBom))
        .parseKeyValue(std::forward<TCallback>(callback));
  }

 protected:
  ~JsonBufferBase() {}

 private:
  TImpl *impl() {
    return static_cast<TImpl *>(this);
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
