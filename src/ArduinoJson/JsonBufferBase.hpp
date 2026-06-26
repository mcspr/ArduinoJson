// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Deserialization/JsonParser.hpp"

namespace ArduinoJson {
namespace Internals {

// User-facing parser API. Generally, everything string-like that is declared via StringTraits is supported.
template <typename TDerived>
class JsonBufferBase : public JsonBuffer {
 public:
  // Allocates and populate a JsonArray from a JSON string.
  //
  // Returns a reference to the new JsonArray or JsonArray::invalid() if the
  // allocation fails.
  template <typename TData>
  JsonArray &parseArray(TData&& json, uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser<Internals::JsonParser>(
      that(), std::forward<TData>(json), nestingLimit).parseArray();
  }

  // Allocates and populate a JsonObject from a JSON string.
  //
  // Returns a reference to the new JsonObject or JsonObject::invalid() if the
  // allocation fails.
  template <typename TData>
  JsonObject &parseObject(TData&& json, uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser<Internals::JsonParser>(
      that(), std::forward<TData>(json), nestingLimit).parseObject();
  }

  // Generalized version of parseArray() and parseObject(), also works for
  // integral types.
  template <typename TData>
  JsonVariant parse(TData&& json, uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser<Internals::JsonParser>(
      that(), std::forward<TData>(json), nestingLimit).parseVariant();
  }

  // Simplified version of parseObject(..., nestingLimit=1) w/o implicit object allocation
  // CAUTION: only works for plain objects {key1: val1, key2: val2, ...}
  template <typename TData, typename TCallback>
  JsonVariant parseKeyValue(TData&& json, TCallback&& callback) {
    return Internals::makeParser<Internals::JsonKeyValueParser>(
      that(), std::forward<TData>(json), 1).parseKeyValue(std::forward<TCallback>(callback));
  }

 protected:
  ~JsonBufferBase() {}

 private:
  TDerived *that() {
    return static_cast<TDerived *>(this);
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
