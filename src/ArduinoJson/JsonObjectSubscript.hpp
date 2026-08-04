// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonSubscriptBase.hpp"
#include "Data/StringRef.hpp"
#include "Data/StringViewRef.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename>
class JsonMutableObjectSubscript;

template <typename>
class JsonConstObjectSubscript;

template <typename TKey>
using TJsonObjectSubscriptKeyType =
  typename StringRefWrapperHelper<TKey>::wrapper_type;

template <typename TKey>
class JsonMutableObjectSubscript final
    : public JsonSubscriptBase<JsonObject, JsonMutableObjectSubscript<TKey> > {

 public:
  JsonMutableObjectSubscript() noexcept;

  template <typename TKeyRef>
  JsonMutableObjectSubscript(JsonObject& object, TKeyRef&& key);

  template <typename TRef>
  JsonMutableObjectSubscript(JsonObject& object, StringRefWrapper<TRef> key);

  // allow to construct the object, but disallow changes after construction

  JsonMutableObjectSubscript(const JsonMutableObjectSubscript &) = default;
  JsonMutableObjectSubscript(JsonMutableObjectSubscript &&) = default;

  JsonMutableObjectSubscript(const JsonConstObjectSubscript<TKey> &) = delete;
  JsonMutableObjectSubscript(JsonConstObjectSubscript<TKey> &&) = delete;

  // class copy / move implicitly converts into JsonVariant and assigns

  JsonMutableObjectSubscript& operator=(const JsonMutableObjectSubscript &);
  JsonMutableObjectSubscript& operator=(JsonMutableObjectSubscript &&);

  JsonMutableObjectSubscript& operator=(const JsonConstObjectSubscript<TKey> &);
  JsonMutableObjectSubscript& operator=(JsonConstObjectSubscript<TKey> &&);

  template <typename TValue>
  JsonMutableObjectSubscript& operator=(TValue&&);

  // forwarding methods for the bound key and JsonObject ref

  bool success() const;

  // aka JsonObject::get<TValue>(key)
  template <typename TValue>
  typename JsonVariantAs<TValue>::type as() const;

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
class JsonConstObjectSubscript final
    : public JsonSubscriptBase<const JsonObject, JsonConstObjectSubscript<TKey> > {

 public:
  JsonConstObjectSubscript() noexcept;

  template <typename TKeyRef>
  JsonConstObjectSubscript(const JsonObject& object, TKeyRef&& key);

  template <typename TRef>
  JsonConstObjectSubscript(const JsonObject& object, StringRefWrapper<TRef> key);

  // allow to construct the object, but disallow changes after construction

  JsonConstObjectSubscript(const JsonConstObjectSubscript &) = default;
  JsonConstObjectSubscript(JsonConstObjectSubscript &&) = default;

  JsonConstObjectSubscript(const JsonMutableObjectSubscript<TKey> &);
  JsonConstObjectSubscript(JsonMutableObjectSubscript<TKey> &&);

  // class copy / move implicitly converts into JsonVariant and assigns

  JsonConstObjectSubscript& operator=(const JsonMutableObjectSubscript<TKey> &) = delete;
  JsonConstObjectSubscript& operator=(JsonMutableObjectSubscript<TKey> &&) = delete;

  JsonConstObjectSubscript& operator=(const JsonConstObjectSubscript &) = delete;
  JsonConstObjectSubscript& operator=(JsonConstObjectSubscript<TKey> &&) = delete;

  template <typename TValue>
  JsonConstObjectSubscript& operator=(TValue &&) = delete;

  // forwarding methods for the bound key and JsonObject ref

  bool success() const;

  // aka JsonObject::get<TValue>(key)
  template <typename TValue>
  typename JsonVariantAsConst<TValue>::type as() const;

  // aka JsonObject::is<TValue>(key)
  template <typename TValue>
  bool is() const;

 private:
  const JsonObject& _object;
  TJsonObjectSubscriptKeyType<TKey> _key;
};

template <typename TJsonObject, typename TKey>
struct JsonObjectSubscriptHelper {
  typedef TJsonObjectSubscriptKeyType<TKey> key_type;
  typedef Conditional<IsConst<TJsonObject>::value,
    JsonConstObjectSubscript<typename key_type::string_type>,
    JsonMutableObjectSubscript<typename key_type::string_type>> subscript_type;
};

}  // namespace Internals
}  // namespace ArduinoJson
