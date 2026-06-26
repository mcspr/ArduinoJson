// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonArray.hpp"
#include "JsonObject.hpp"
#include "JsonObjectSubscript.hpp"

namespace ArduinoJson {

template <typename TKey>
inline JsonArray &JsonObject::createNestedArray_impl(TKey&& key) {
  JsonArray &array = _buffer->createArray();
  if (set(std::forward<TKey>(key), array))
    return array;
  return JsonArray::invalid();
}

template <typename TKey>
inline JsonObject &JsonObject::createNestedObject_impl(TKey&& key) {
  JsonObject &object = _buffer->createObject();
  if (set(std::forward<TKey>(key), object))
    return object;
  return JsonObject::invalid();
}
template <typename TChar, size_t Size>
Internals::JsonObjectSubscript<TChar*>
inline JsonObject::operator[](TChar (&key)[Size]) {
  return this->operator[](&key[0]);
}

template <typename TChar, size_t Size>
const Internals::JsonObjectSubscript<TChar*>
inline JsonObject::operator[](TChar (&key)[Size]) const {
  return this->operator[](&key[0]);
}
}  // namespace ArduinoJson
