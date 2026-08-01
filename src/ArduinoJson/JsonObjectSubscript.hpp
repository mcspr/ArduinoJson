// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Configuration.hpp"

#include "JsonVariantBase.hpp"
#include "Data/StringView.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename TKey>
using TJsonObjectSubscriptKeyType =
  typename StringRefWrapperHelper<TKey>::wrapper_type;

template <typename TKey>
class JsonObjectSubscript final
    : public JsonVariantBase<JsonObjectSubscript<TKey> > {

 public:
  JsonObjectSubscript() noexcept;

  template <typename TKeyRef>
  JsonObjectSubscript(JsonObject& object, TKeyRef&& key);

  template <typename TRef>
  JsonObjectSubscript(JsonObject& object, StringRefWrapper<TRef> key);

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
  JsonObjectSubscript& operator=(const JsonObjectSubscript&);

  // forwarding methods for the bound key and JsonObject ref

  bool success() const;

  // aka JsonObject::get<TValue>(key)
  template <typename TValue>
  typename JsonVariantAs<TValue>::type as();

  template <typename TValue>
  typename JsonVariantAsConst<TValue>::type as() const {
    return const_cast<JsonObjectSubscript *>(this)->
      as<typename JsonVariantAsConst<TValue>::type>();
  }

  // aka JsonObject::is<TValue>(key)
  template <typename TValue>
  bool is() const;

  // aka JsonObject::set<TValue>(key, value)
  template <typename TValue>
  bool set(TValue&& value);

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
