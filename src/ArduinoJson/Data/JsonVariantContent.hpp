// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonFloat.hpp"
#include "JsonInteger.hpp"
#include "JsonNull.hpp"

namespace ArduinoJson {

// Forward declarations
class JsonArray;
class JsonObject;

namespace Internals {
// A union that defines the actual content of a JsonVariant.
// The enum JsonVariantType determines which member is in use.
union JsonVariantContent {
  JsonVariantContent() :
    undefined{}
  {}

  explicit JsonVariantContent(JsonNull) :
    null{}
  {}

  explicit JsonVariantContent(JsonFloat value) :
    asFloat(value)
  {}

  explicit JsonVariantContent(JsonUInt value) :
    asInteger(value)
  {}

  explicit JsonVariantContent(const char* value) :
    asString(value)
  {}

  explicit JsonVariantContent(JsonArray* ptr) :
    asArray(ptr)
  {}

  explicit JsonVariantContent(JsonObject* ptr) :
    asObject(ptr)
  {}

  char undefined;
  char null;             // value is absent, but explicitly so
  JsonFloat asFloat;     // used for double and float
  JsonUInt asInteger;    // used for bool, char, short, int and longs
  const char* asString;  // asString can be null
  JsonArray* asArray;    // asArray cannot be null
  JsonObject* asObject;  // asObject cannot be null
};

}  // namespace Internals
}  // namespace ArduinoJson
