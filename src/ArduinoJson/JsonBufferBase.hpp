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
  template <typename TJson,
    typename TParseString = JsonParserImpl::ParseStringImpl,
    template <typename, typename, typename> class TJsonParser = Internals::JsonParser,
    typename TDefaultDeserializationOptions = DefaultDeserializationOptions>
  JsonVariant parse(TJson&& json, DeserializationOptionsImpl<TDefaultDeserializationOptions> deserializationOptions) {
    using TDeserializationOptions = DeserializationOptionsImpl<TDefaultDeserializationOptions>;
    return Internals::makeParser<TJsonParser, TParseString, TDeserializationOptions>(
      impl(), std::forward<TJson>(json), deserializationOptions).parseVariant();
  }

  template <typename TJson,
    typename TParseString = JsonParserImpl::ParseStringImpl,
    template <typename, typename, typename> class TJsonParser = Internals::JsonParser,
    typename TDefaultDeserializationOptions = DefaultDeserializationOptions>
  JsonVariant parse(TJson&& json) {
    using TDeserializationOptions = DeserializationOptionsImpl<TDefaultDeserializationOptions>;
    return parse<TJson, TParseString, TJsonParser, TDefaultDeserializationOptions>(
      std::forward<TJson>(json),
      TDeserializationOptions());
  }

  // Parse and populate a JsonArray from a JSON string, returning a reference to it.
  // Returns JsonArray::invalid() on parsing errors or buffer allocation failures.
  template <typename TJson,
    typename TParseString = JsonParserImpl::ParseStringImpl,
    template <typename, typename, typename> class TJsonParser = Internals::JsonParser,
    typename TDefaultDeserializationOptions = DefaultDeserializationOptions>
  JsonArray& parseArray(TJson&& json, DeserializationOptionsImpl<TDefaultDeserializationOptions> deserializationOptions) {
    using TDeserializationOptions = DeserializationOptionsImpl<TDefaultDeserializationOptions>;
    return Internals::makeParser<TJsonParser, TParseString, TDeserializationOptions>(
      impl(), std::forward<TJson>(json), deserializationOptions).parseArray();
  }

  template <typename TJson,
    typename TParseString = JsonParserImpl::ParseStringImpl,
    template <typename, typename, typename> class TJsonParser = Internals::JsonParser,
    typename TDefaultDeserializationOptions = DefaultDeserializationOptions>
  JsonArray& parseArray(TJson&& json) {
    using TDeserializationOptions = DeserializationOptionsImpl<TDefaultDeserializationOptions>;
    return parseArray<TJson, TParseString, TJsonParser, TDefaultDeserializationOptions>(
      std::forward<TJson>(json),
      TDeserializationOptions());
  }

  // Parse and populate a JsonObject from a JSON string, returning a reference to it.
  // Returns JsonObject::invalid() on parsing errors or buffer allocation failures.
  template <typename TJson,
    typename TParseString = JsonParserImpl::ParseStringImpl,
    template <typename, typename, typename> class TJsonParser = Internals::JsonParser,
    typename TDefaultDeserializationOptions = DefaultDeserializationOptions>
  JsonObject& parseObject(TJson&& json, DeserializationOptionsImpl<TDefaultDeserializationOptions> deserializationOptions) {
    using TDeserializationOptions = DeserializationOptionsImpl<TDefaultDeserializationOptions>;
    return Internals::makeParser<TJsonParser, TParseString, TDeserializationOptions>(
      impl(), std::forward<TJson>(json), deserializationOptions).parseObject();
  }

  template <typename TJson,
    typename TParseString = JsonParserImpl::ParseStringImpl,
    template <typename, typename, typename> class TJsonParser = Internals::JsonParser,
    typename TDefaultDeserializationOptions = DefaultDeserializationOptions>
  JsonObject& parseObject(TJson&& json) {
    using TDeserializationOptions = DeserializationOptionsImpl<TDefaultDeserializationOptions>;
    return parseObject<TJson, TParseString, TJsonParser, TDefaultDeserializationOptions>(
      std::forward<TJson>(json), TDeserializationOptions());
  }

  // Simplified version of parseObject(..., {nestingLimit=1}) w/o implicit object allocation
  // CAUTION: only works for flat objects {key1: val1, key2: val2, ...}
  // Returns false on parsing errors and intermediate buffer allocation failures.
  template <typename TJson, typename TCallback,
    typename TParseString = JsonParserImpl::ParseStringImpl,
    template <typename, typename, typename> class TJsonParser = Internals::JsonKeyValueParser,
    typename TDefaultDeserializationOptions = DefaultDeserializationOptions>
  bool parseKeyValue(TJson&& json, TCallback&& callback,
      DeserializationOptionsImpl<TDefaultDeserializationOptions> deserializationOptions)
  {
    using TDeserializationOptions = DeserializationOptionsImpl<TDefaultDeserializationOptions>;
    return Internals::makeParser<TJsonParser, TParseString, TDeserializationOptions>(
      impl(),
      std::forward<TJson>(json),
      TDeserializationOptions(1,
        deserializationOptions.enableComments,
        deserializationOptions.skipBom))
      .parseKeyValue(std::forward<TCallback>(callback));
  }

  template <typename TJson, typename TCallback,
    typename TParseString = JsonParserImpl::ParseStringImpl,
    template <typename, typename, typename> class TJsonParser = Internals::JsonKeyValueParser,
    typename TDefaultDeserializationOptions = DefaultDeserializationOptions>
  bool parseKeyValue(TJson&& json, TCallback&& callback) {
    using TDeserializationOptions = DeserializationOptionsImpl<TDefaultDeserializationOptions>;
    return parseKeyValue<TJson, TCallback, TParseString, TJsonParser, TDefaultDeserializationOptions>(
      std::forward<TJson>(json),
      std::forward<TCallback>(callback),
      TDeserializationOptions(1));
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
