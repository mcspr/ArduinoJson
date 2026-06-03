// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Data/JsonVariantAs.hpp"
#include "Polyfills/attributes.hpp"

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

template <typename TImpl>
class JsonVariantCasts {
 public:
  // Gets the variant as an array.
  // Returns a reference to the JsonArray or JsonArray::invalid() if the
  // variant
  // is not an array.
  ARDUINOJSON_FORCE_INLINE operator JsonArray &() const {
    return impl()->template as<JsonArray &>();
  }

  // Gets the variant as an object.
  // Returns a reference to the JsonObject or JsonObject::invalid() if the
  // variant is not an object.
  ARDUINOJSON_FORCE_INLINE operator JsonObject &() const {
    return impl()->template as<JsonObject &>();
  }

  template <typename T>
  ARDUINOJSON_FORCE_INLINE operator T() const {
    return impl()->template as<T>();
  }

  // Avoid empty T* being used as an output type
  // Triggers substitution match earlier than through EnableIf
  operator std::nullptr_t() const = delete;

 private:
  const TImpl *impl() const {
    return static_cast<const TImpl *>(this);
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
