// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../JsonArray.hpp"
#include "../JsonArraySubscript.hpp"
#include "../JsonObject.hpp"
#include "../JsonObjectSubscript.hpp"
#include "../JsonVariant.hpp"

#include "JsonSerializer.hpp"

template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::serialize(
    const JsonArray& array, Writer& writer) {
  writer.beginArray();

  JsonArray::const_iterator it = array.begin();
  while (it != array.end()) {
    serialize(*it, writer);

    ++it;
    if (it == array.end()) break;

    writer.writeComma();
  }

  writer.endArray();
}

template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::serialize(
    JsonArraySubscript arraySubscript, Writer& writer) {
  serialize(arraySubscript.as<JsonVariant>(), writer);
}

template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::serialize(
    const JsonObject& object, Writer& writer) {
  writer.beginObject();

  JsonObject::const_iterator it = object.begin();
  while (it != object.end()) {
    writer.writeString(it->key);
    writer.writeColon();
    serialize(it->value, writer);

    ++it;
    if (it == object.end()) break;

    writer.writeComma();
  }

  writer.endObject();
}

template <typename Writer>
template <typename TKey>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::serialize(
    const JsonObjectSubscript<TKey>& objectSubscript, Writer& writer) {

  serialize(objectSubscript.template as<JsonVariant>(), writer);
}

template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::VariantVisitor::Operator(JsonNull) {
  _writer.writeNull();
}

template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::VariantVisitor::Operator(bool value) {
  _writer.writeBoolean(value);
}

template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::VariantVisitor::Operator(JsonObject* object) {
  serialize(*object, _writer);
}

template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::VariantVisitor::Operator(JsonArray* array) {
  serialize(*array, _writer);
}

template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::VariantVisitor::Operator(JsonFloat value) {
  _writer.writeFloat(value);
}

template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::VariantVisitor::Operator(JsonInteger value) {
  _writer.writeInteger(value);
}

template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::VariantVisitor::Operator(JsonUnsignedInteger value) {
  _writer.writeUnsignedInteger(value);
}

template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::VariantVisitor::Operator(JsonVariantString str) {
  if (str.parsed)
    _writer.writeString(str.data);
  else
    _writer.writeRaw(str.data);
}

template <typename Writer>
inline void ArduinoJson::Internals::JsonSerializer<Writer>::serialize(
    JsonVariant variant, Writer& writer) {

  variant.visit<void>(VariantVisitor(writer));
}
