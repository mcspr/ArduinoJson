// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonArray.hpp"
#include "JsonObject.hpp"
#include "JsonObjectSubscript.hpp"

namespace ArduinoJson {

template <typename TStringRef>
inline JsonArray &JsonObject::createNestedArray_impl(TStringRef key) {
  JsonArray &array = _buffer->createArray();
  if (set(key, array))
    return array;
  else
    return JsonArray::invalid();
}

template <typename TStringRef>
inline JsonObject &JsonObject::createNestedObject_impl(TStringRef key) {
  JsonObject &object = _buffer->createObject();
  if (set(key, object))
    return object;
  else
    return JsonObject::invalid();
}
}  // namespace ArduinoJson
