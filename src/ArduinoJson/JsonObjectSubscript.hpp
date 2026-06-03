// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Configuration.hpp"

#include "JsonVariantBase.hpp"

#include "TypeTraits/EnableIf.hpp"
#include "TypeTraits/IsPointer.hpp"
#include "TypeTraits/RemoveReference.hpp"

#include "Data/StringRef.hpp"

namespace ArduinoJson {
namespace Internals {

// strip useless qualifiers before attempting to create a key type

template <typename TKey, typename = void>
struct JsonObjectSubscriptHelper {
  typedef TKey raw_key_type;
  typedef TKey key_type;
  typedef StringRefWrapper<key_type> subscript_key_type;
};

template <typename TKey>
struct JsonObjectSubscriptHelper<TKey,
  typename EnableIf<IsPointer<typename RemoveReference<TKey>::type>::value, void>::type> {

  typedef TKey raw_key_type;
  typedef typename RemoveConstReference<TKey>::type key_type;
  typedef StringRefWrapper<key_type> subscript_key_type;
};

template <typename TKey>
class JsonObjectSubscript final
    : public JsonVariantBase<JsonObjectSubscript<TKey> > {

 public:
  // *Always* attached to some JsonObject instance

  JsonObjectSubscript() :
    _object(JsonObject::invalid()),
    _key()
  {}

  template <typename TKeyRef>
  JsonObjectSubscript(JsonObject& object, TKeyRef&& key) :
    _object(object),
    _key(std::forward<TKeyRef>(key))
  {}

  // Allow to construct the object, but disallow changes after construction

  JsonObjectSubscript(const JsonObjectSubscript &) = default;
  JsonObjectSubscript(JsonObjectSubscript &&) = default;

  // TValue = bool, char, long, int, short, float, double,
  //          char*, char[], std::string, String, JsonArray, JsonObject
  template <typename TValue>
  JsonObjectSubscript& operator=(TValue&& src) {
    _object.set(_key.get(), std::forward<TValue>(src));
    return *this;
  }

  template <typename TChar, size_t Size>
  JsonObjectSubscript& operator=(TChar (&src)[Size]) {
    _object.set(_key.get(), src);
    return *this;
  }

  // class copy is disallowed, interpret it as an assignment operation
  JsonObjectSubscript& operator=(const JsonObjectSubscript& other) {
    _object.set(_key.get(), other);
    return *this;
  }

  bool success() const {
    return _object.containsKey(_key.get());
  }

  template <typename TValue>
  ARDUINOJSON_FORCE_INLINE typename JsonVariantAs<TValue>::type as() const {
    return _object.get<TValue>(_key.get());
  }

  template <typename TValue>
  ARDUINOJSON_FORCE_INLINE
  bool is() const {
    return _object.is<typename JsonVariantAs<TValue>::type>(_key.get());
  }

  // TValue = bool, char, long, int, short, float, double, RawJson, JsonVariant,
  //          char*, char[], const __FlashStringHelper*, std::string, String, JsonArray, JsonObject
  template <typename TValue>
  ARDUINOJSON_FORCE_INLINE bool set(TValue&& value) {
    return _object.set(_key.get(), std::forward<TValue>(value));
  }

  template <typename TChar, size_t Size>
  ARDUINOJSON_FORCE_INLINE bool set(TChar (&value)[Size]) {
    return _object.set(_key.get(), value);
  }

 private:
  typedef typename StringRefWrapperHelper<TKey>::wrapper_type TKeyType;

  JsonObject& _object;
  TKeyType _key;
};

#if ARDUINOJSON_ENABLE_STD_STREAM
template <typename TKey>
inline std::ostream& operator<<(std::ostream& os,
                                const JsonObjectSubscript<TKey>& source) {
  return source.printTo(os);
}
#endif
}  // namespace Internals
}  // namespace ArduinoJson
