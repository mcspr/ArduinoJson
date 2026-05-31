// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Configuration.hpp"
#include "JsonVariantBase.hpp"
#include "TypeTraits/EnableIf.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename TStringRef>
class JsonObjectSubscript final
    : public JsonVariantBase<JsonObjectSubscript<TStringRef> > {
  typedef JsonObjectSubscript<TStringRef> this_type;

 public:
  // *Always* attached to some JsonObject instance

  JsonObjectSubscript() = delete;

  JsonObjectSubscript(JsonObject& object, TStringRef key)
      : _object(object), _key(key) {}

  // Allow to construct the object, but disallow changes after construction
  JsonObjectSubscript(const JsonObjectSubscript &) = default;
  JsonObjectSubscript(JsonObjectSubscript &&) = default;

  // class copy is disallowed, interpret it as an assignment operation
  JsonObjectSubscript& operator=(const JsonObjectSubscript& other) {
    _object.set(_key, other);
    return *this;
  }

  // Set the specified value
  //
  // operator=(const TValue&);
  // TValue = bool, char, long, int, short, float, double,
  //          std::string, String, JsonArray, JsonObject
  template <typename TValue>
  FORCE_INLINE typename EnableIf<!IsArray<TValue>::value, this_type&>::type
  operator=(const TValue& src) {
    _object.set(_key, src);
    return *this;
  }
  //
  // operator=(TValue);
  // TValue = char*, const char*, const FlashStringHelper*
  template <typename TValue>
  FORCE_INLINE this_type& operator=(TValue* src) {
    _object.set(_key, src);
    return *this;
  }

  FORCE_INLINE bool success() const {
    return _object.containsKey(_key);
  }

  template <typename TValue>
  FORCE_INLINE typename JsonVariantAs<TValue>::type as() const {
    return _object.get<TValue>(_key);
  }

  template <typename TValue>
  FORCE_INLINE bool is() const {
    return _object.is<TValue>(_key);
  }

  // Sets the specified value.
  //
  // bool set(const TValue&);
  // TValue = bool, char, long, int, short, float, double, RawJson, JsonVariant,
  //          std::string, String, JsonArray, JsonObject
  template <typename TValue>
  FORCE_INLINE typename EnableIf<!IsArray<TValue>::value, bool>::type set(
      const TValue& value) {
    return _object.set(_key, value);
  }
  //
  // bool set(TValue);
  // TValue = char*, const char, const FlashStringHelper*
  template <typename TValue>
  FORCE_INLINE bool set(const TValue* value) {
    return _object.set(_key, value);
  }
  //
  // bool set(TValue, uint8_t decimals);
  // TValue = float, double
  template <typename TValue>
  DEPRECATED("Second argument is not supported anymore")
  FORCE_INLINE bool set(const TValue& value, uint8_t) {
    return _object.set(_key, value);
  }

 private:
  JsonObject& _object;
  TStringRef _key;
};

#if ARDUINOJSON_ENABLE_STD_STREAM
template <typename TStringRef>
inline std::ostream& operator<<(std::ostream& os,
                                const JsonObjectSubscript<TStringRef>& source) {
  return source.printTo(os);
}
#endif
}  // namespace Internals
}  // namespace ArduinoJson
