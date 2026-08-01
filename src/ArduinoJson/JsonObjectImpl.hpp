// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonArray.hpp"
#include "JsonObject.hpp"
#include "JsonObjectSubscript.hpp"

#include "Polyfills/attributes.hpp"
#include "StringTraits/StringTraits.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename TKey>
class JsonObjectSubscript;

// *Always* attached to some JsonObject instance
template <typename TKey>
inline JsonObjectSubscript<TKey>::JsonObjectSubscript() noexcept :
  _object(JsonObject::invalid()),
  _key()
{}

template <typename TKey>
template <typename TKeyRef>
inline JsonObjectSubscript<TKey>::JsonObjectSubscript(JsonObject& object, TKeyRef&& key) :
  _object(object),
  _key(MakeStringRef(std::forward<TKeyRef>(key)))
{}

template <typename TKey>
template <typename TRef>
inline JsonObjectSubscript<TKey>::JsonObjectSubscript(JsonObject& object, StringRefWrapper<TRef> key) :
  _object(object),
  _key(std::move(key))
{}

template <typename TKey>
inline JsonObjectSubscript<TKey>&
ARDUINOJSON_FORCE_INLINE JsonObjectSubscript<TKey>::operator=(const JsonObjectSubscript<TKey>& other) {
  _object.set(_key.get(), other.template as<JsonVariant>());
  return *this;
}

template <typename TKey>
inline bool JsonObjectSubscript<TKey>::success() const {
  return _object.containsKey(_key.get());
}

template <typename TKey>
template <typename TValue>
inline typename JsonVariantAs<TValue>::type
ARDUINOJSON_FORCE_INLINE JsonObjectSubscript<TKey>::as() {
  return _object.get<TValue>(_key.get());
}

template <typename TKey>
template <typename TValue>
inline bool
ARDUINOJSON_FORCE_INLINE JsonObjectSubscript<TKey>::is() const {
  return _object.is<TValue>(_key.get());
}

template <typename TKey>
template <typename TValue>
inline bool
ARDUINOJSON_FORCE_INLINE JsonObjectSubscript<TKey>::set(TValue&& value) {
  return _object.set(_key.get(), std::forward<TValue>(value));
}

}

template <typename TKey>
inline JsonObject::iterator JsonObject::find_impl(TKey key) {
  for (auto it = begin(); it != end(); ++it) {
    const auto ptr = it->key.as<const char*>();
    const auto len = Internals::Strings::Length::Operator(ptr);
    if (Internals::StringTraits<TKey>::Equals::Operator(key, ptr, len))
      return it;
  }
  return end();
}

template <typename TValue, typename TKey>
inline TValue JsonObject::get_impl(TKey key) {
    iterator it = find_impl(std::move(key));
    return it != end() ? it->value.as<TValue>()
                       : Internals::JsonVariantDefault<TValue>::get();
}

template <typename TKey, typename TValue>
inline bool JsonObject::set_impl(TKey key, TValue value) {
  // when creating a key, prune failed list entry before returning
  bool out = false;

  // search for existing or add another kv object
  auto it = find_impl(Internals::MakeStringRef(key.get()));
  if (it == end()) {
    it = add();
    if (it == end())
      return false;

    if (!Internals::ValueSaver<TKey>::save(
      _buffer, it->key, std::move(key)))
    {
      remove(it);
      return false;
    }

    out = true;
  }

  if (it != end()) {
    if (!Internals::ValueSaver<TValue>::save(
      _buffer, it->value, std::move(value)))
    {
      if (out)
        remove(it);

      return false;
    }

    out = true;
  }

  return out;
}

template <typename TValue, typename TKey>
bool JsonObject::is_impl(TKey key) const {
    const_iterator it = find_impl(std::move(key));
    return it != end() ? it->value.is<TValue>() : false;
}

template <typename TKey>
inline JsonArray& JsonObject::createNestedArray_impl(TKey key) {
  auto &ref = _buffer->createArray();
  if (set_impl(std::move(key), JsonVariant(ref)))
    return ref;
  return JsonArray::invalid();
}

template <typename TKey>
inline JsonObject& JsonObject::createNestedObject_impl(TKey key) {
  JsonObject &ref = _buffer->createObject();
  if (set_impl(std::move(key), JsonVariant(ref)))
    return ref;
  return JsonObject::invalid();
}

template <typename TKey>
typename Internals::JsonObjectSubscriptHelper<TKey>::subscript_type
inline JsonObject::operator[](TKey&& key) {
  return typename Internals::JsonObjectSubscriptHelper<TKey>::subscript_type(
    *this, std::forward<TKey>(key));
}

template <typename TKey>
const typename Internals::JsonObjectSubscriptHelper<TKey>::subscript_type
inline JsonObject::operator[](TKey&& key) const {
  return typename Internals::JsonObjectSubscriptHelper<TKey>::subscript_type(
    const_cast<JsonObject&>(*this), std::forward<TKey>(key));
}
}  // namespace ArduinoJson
