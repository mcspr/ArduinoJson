// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include <cstdint>

namespace ArduinoJson {
class JsonArray;
class JsonObject;

namespace Internals {

// Enumerated type to know the current type of a JsonVariant.
// The value determines which member of JsonVariantContent is used.
enum class JsonVariantType : uint8_t {
  JSON_UNDEFINED,         // JsonVariant has not been initialized
  JSON_NULL,              // JsonVariant contains a null
  JSON_BOOLEAN,           // JsonVariant stores a bool
  JSON_OBJECT,            // JsonVariant stores a pointer to a JsonObject
  JSON_ARRAY,             // JsonVariant stores a pointer to a JsonArray
  JSON_FLOAT,             // JsonVariant stores a JsonFloat
  JSON_SIGNED_INTEGER,    // JsonVariant stores a JsonInteger
  JSON_UNSIGNED_INTEGER,  // JsonVariant stores a JsonUnsignedIntger
  JSON_STRING,            // JsonVariant stores a const char* to either parsed or unparsed string
  JSON_STRING_BUFFER,     // JsonVariant stores a small const char* data, either parsed or unparsed string
};

}  // namespace Internals
}  // namespace ArduinoJson
