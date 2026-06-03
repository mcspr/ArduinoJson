// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Deserialization/JsonParser.hpp"

namespace ArduinoJson {
namespace Internals {
template <typename TDerived>

// User-facing parser API. Generally, everything string-like that is declared via StringTraits is supported.
class JsonBufferBase : public JsonBuffer {
 public:
  // Allocates and populate a JsonArray from a JSON string.
  //
  // Returns a reference to the new JsonArray or JsonArray::invalid() if the
  // allocation fails.
  template <typename TData>
  JsonArray &parseArray(TData&& json, uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser(that(), std::forward<TData>(json), nestingLimit).parseArray();
  }

  // Allocates and populate a JsonObject from a JSON string.
  //
  // Returns a reference to the new JsonObject or JsonObject::invalid() if the
  // allocation fails.
  template <typename TData>
  JsonObject &parseObject(TData&& json, uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser(that(), std::forward<TData>(json), nestingLimit).parseObject();
  }

  // Generalized version of parseArray() and parseObject(), also works for
  // integral types.
  template <typename TData>
  JsonVariant parse(TData&& json, uint8_t nestingLimit = ARDUINOJSON_DEFAULT_NESTING_LIMIT) {
    return Internals::makeParser(that(), std::forward<TData>(json), nestingLimit).parseVariant();
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
