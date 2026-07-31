// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonArray.hpp"
#include "JsonObject.hpp"
#include "JsonObjectSubscript.hpp"

#include "StringTraits/StringTraits.hpp"

namespace ArduinoJson {

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
inline TValue JsonObject::get_impl(TKey key) const {
    const_iterator it = find_impl(std::move(key));
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

template <typename TKey>
inline JsonArray &JsonObject::createNestedArray_impl(TKey key) {
  auto &ref = _buffer->createArray();
  if (set_impl(std::move(key), JsonVariant(ref)))
    return ref;
  return JsonArray::invalid();
}

template <typename TKey>
inline JsonObject &JsonObject::createNestedObject_impl(TKey key) {
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
typename Internals::JsonObjectSubscriptHelper<TKey>::subscript_type
inline JsonObject::operator[](TKey&& key) const {
  return typename Internals::JsonObjectSubscriptHelper<TKey>::subscript_type(
    const_cast<JsonObject&>(*this), std::forward<TKey>(key));
}
}  // namespace ArduinoJson
