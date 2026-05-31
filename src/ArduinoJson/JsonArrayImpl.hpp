// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonArray.hpp"
#include "JsonArraySubscript.hpp"
#include "JsonObject.hpp"

namespace ArduinoJson {

inline JsonArray &JsonArray::createNestedArray() {
  JsonArray &array = _buffer->createArray();
  add(array);
  return array;
}

inline JsonObject &JsonArray::createNestedObject() {
  JsonObject &object = _buffer->createObject();
  add(object);
  return object;
}
}  // namespace ArduinoJson
