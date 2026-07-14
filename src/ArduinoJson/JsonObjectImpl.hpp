// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonArray.hpp"
#include "JsonObject.hpp"
#include "JsonObjectSubscript.hpp"

namespace ArduinoJson {

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
