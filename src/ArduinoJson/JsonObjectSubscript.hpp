// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Configuration.hpp"

#include "JsonVariantBase.hpp"

#include "Data/JsonNull.hpp"
#include "Data/StringRef.hpp"
#include "Data/StringView.hpp"

#include "TypeTraits/EnableIf.hpp"
#include "TypeTraits/IsPointer.hpp"
#include "TypeTraits/IsFloatingPoint.hpp"
#include "TypeTraits/IsIntegral.hpp"
#include "TypeTraits/RemoveReference.hpp"

#include "Data/StringRef.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename TKey>
using TJsonObjectSubscriptKeyType =
  typename StringRefWrapperHelper<TKey>::wrapper_type;

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
    _key(MakeStringRef(std::forward<TKeyRef>(key)))
  {}

  template <typename TRef>
  JsonObjectSubscript(JsonObject& object, StringRefWrapper<TRef> key) :
    _object(object),
    _key(key)
  {}

  // Allow to construct the object, but disallow changes after construction

  JsonObjectSubscript(const JsonObjectSubscript &) = default;
  JsonObjectSubscript(JsonObjectSubscript &&) = default;

  // Everything else is interpreted as object assignment w/ the key attached to the subscript object
  template <typename TValue>
  ARDUINOJSON_FORCE_INLINE JsonObjectSubscript& operator=(TValue&& src) {
    set(std::forward<TValue>(src));
    return *this;
  }

  // class copy is disallowed, interpret it as an assignment operation
  ARDUINOJSON_FORCE_INLINE JsonObjectSubscript& operator=(const JsonObjectSubscript& other) {
    _object.set_impl(_key, other.template as<JsonVariant>());
    return *this;
  }

  bool success() const {
    return _object.contains_impl(_key);
  }

  template <typename TValue>
  ARDUINOJSON_FORCE_INLINE typename JsonVariantAs<TValue>::type as() const {
    return _object.get_impl<typename JsonVariantAs<TValue>::type>(_key);
  }

  template <typename TValue>
  ARDUINOJSON_FORCE_INLINE
  bool is() const {
    return _object.is_impl<typename JsonVariantAs<TValue>::type>(_key);
  }

  template <typename TValue>
  ARDUINOJSON_FORCE_INLINE bool set(TValue&& value) {
    return _object.set(_key.get(), std::forward<TValue>(value));
  }

 private:
  JsonObject& _object;
  TJsonObjectSubscriptKeyType<TKey> _key;
};

template <typename TKey>
struct JsonObjectSubscriptHelper {
  typedef TJsonObjectSubscriptKeyType<TKey> key_type;
  typedef JsonObjectSubscript<typename key_type::string_type> subscript_type;
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
