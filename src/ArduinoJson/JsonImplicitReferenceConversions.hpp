// MIT License

#pragma once

#include "Polyfills/attributes.hpp"

namespace ArduinoJson {

class JsonArray;
class JsonObject;

namespace Internals {

// JsonImplicitConversions but only for references
template <typename TImpl>
class JsonImplicitReferenceConversions {
 public:
  // Returns a reference to the contained JsonArray or JsonArray::invalid()
  // if this variant is not an array.
  ARDUINOJSON_FORCE_INLINE operator const JsonArray &() const {
    return impl()->template as<const JsonArray &>();
  }

  ARDUINOJSON_FORCE_INLINE operator JsonArray &() {
    return impl()->template as<JsonArray &>();
  }

  // Returns a reference to the contained JsonObject or JsonObject::invalid()
  // if this variant is not an object.
  ARDUINOJSON_FORCE_INLINE operator const JsonObject &() const {
    return impl()->template as<const JsonObject &>();
  }

  ARDUINOJSON_FORCE_INLINE operator JsonObject &() {
    return impl()->template as<JsonObject &>();
  }

 private:
  const TImpl* impl() const {
    return static_cast<const TImpl *>(this);
  }

  TImpl* impl() {
    return static_cast<TImpl *>(this);
  }
};

// JsonImplicitReferenceConversions but only for constant references
template <typename TImpl>
class JsonImplicitConstReferenceConversions {
 public:
  ARDUINOJSON_FORCE_INLINE operator const JsonArray &() const {
    return impl()->template as<const JsonArray &>();
  }

  ARDUINOJSON_FORCE_INLINE operator const JsonArray &() {
    return impl()->template as<const JsonArray &>();
  }

  ARDUINOJSON_FORCE_INLINE operator const JsonObject &() const {
    return impl()->template as<const JsonObject &>();
  }

  ARDUINOJSON_FORCE_INLINE operator const JsonObject &() {
    return impl()->template as<const JsonObject &>();
  }

 private:
  const TImpl* impl() const {
    return static_cast<const TImpl *>(this);
  }

  TImpl* impl() {
    return static_cast<TImpl *>(this);
  }
};

}  // namespace Internals
}  // namespace ArduinoJson
