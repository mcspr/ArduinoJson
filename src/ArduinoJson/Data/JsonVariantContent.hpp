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
  JsonVariantContent() noexcept :
    undefined{}
  {}

  explicit JsonVariantContent(JsonNull) noexcept :
    null{}
  {}

  explicit JsonVariantContent(JsonFloat value) noexcept :
    asFloat(value)
  {}

  explicit JsonVariantContent(JsonUInt value) noexcept :
    asInteger(value)
  {}

  explicit JsonVariantContent(const char* value) noexcept :
    asString(value)
  {}

  explicit JsonVariantContent(JsonArray* ptr) noexcept :
    asArray(ptr)
  {}

  explicit JsonVariantContent(JsonObject* ptr) noexcept :
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
