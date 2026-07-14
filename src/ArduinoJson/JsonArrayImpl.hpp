// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonArray.hpp"
#include "JsonObject.hpp"

namespace ArduinoJson {

inline JsonArray &JsonArray::createNestedArray() {
  JsonArray &ref = _buffer->createArray();
  add(JsonVariant(ref));
  return ref;
}

inline JsonObject &JsonArray::createNestedObject() {
  JsonObject &ref = _buffer->createObject();
  add(JsonVariant(ref));
  return ref;
}
}  // namespace ArduinoJson
