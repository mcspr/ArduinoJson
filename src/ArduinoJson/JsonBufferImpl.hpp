// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Data/List.hpp"

#include "JsonArray.hpp"
#include "JsonObject.hpp"

#include "JsonBuffer.hpp"
#include "EmptyJsonBuffer.hpp"

inline void* ArduinoJson::Internals::JsonBufferAllocated::operator new(size_t n, ArduinoJson::JsonBuffer* jsonBuffer) noexcept {
    return jsonBuffer->alloc(n);
}

template <typename T>
inline bool ArduinoJson::Internals::List<T>::success() const {
  return _buffer != EmptyJsonBuffer::instance();
}

inline ArduinoJson::JsonArray& ArduinoJson::JsonBuffer::createArray() {
  JsonArray *ptr = new (this) JsonArray(this);
  return ptr ? *ptr : JsonArray::invalid();
}

inline ArduinoJson::JsonObject& ArduinoJson::JsonBuffer::createObject() {
  JsonObject *ptr = new (this) JsonObject(this);
  return ptr ? *ptr : JsonObject::invalid();
}

inline ArduinoJson::JsonArray& ArduinoJson::JsonArray::invalid() {
  static JsonArray instance(Internals::EmptyJsonBuffer::instance());
  return instance;
}

inline ArduinoJson::JsonObject& ArduinoJson::JsonObject::invalid() {
  static JsonObject instance(Internals::EmptyJsonBuffer::instance());
  return instance;
}
