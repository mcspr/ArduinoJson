// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonFloat.hpp"
#include "JsonInteger.hpp"
#include "JsonNull.hpp"
#include "JsonVariantType.hpp"

namespace ArduinoJson {

// Forward declarations
class JsonArray;
class JsonObject;

namespace Internals {

// Putting the 'type' field in front to avoid storing it in the parent class.
//
// Ensure that these are 'standard-layout' and have a 'common initial sequence', allowing 'type' to be
// considered active regardless of an actual active union member.
//
// > A class is considered to be standard-layout and to have properties described below if and only if it is a POD classA class where all non-static
// > data members have the same access control and certain other conditions are satisfied is known as standard-layout class (see standard-layout class
// > for the list of requirements).
//
// > The common initial sequence of two standard-layout non-union class types is the longest sequence of non-static
// > data members and bit-fields in declaration order, starting with the first such entity in each of the classes, such that
// > - if __has_cpp_attribute(no_unique_address) is not 0, neither entity is declared with [[no_unique_address]] attribute, (since C++20)
// > - corresponding entities have layout-compatible types,
// > - corresponding entities have the same alignment requirements, and
// >   either both entities are bit-fields with the same width or neither is a bit-field.
//
// ...
//
// > In a standard-layout union with an active member of non-union class type T1, it is permitted to read a non-static data member m of another union
// > member of non-union class type T2 provided m is part of the common initial sequence of T1 and T2 (except that reading a volatile member through
// > non-volatile glvalue is undefined).
//
// 'type' is expected to be fairly small (u8 currently)
//
// note that implementation *may* fit some extra data in-between 'type' and any following member fields
// main use-case currently are various flags that would fit in the otherwise unused unnamed struct padding 

// default variant state
struct JsonVariantContentUndefined {
  JsonVariantType type;
};

// value is absent, but explicitly so
struct JsonVariantContentNull {
  JsonVariantType type;
};

struct JsonVariantContentBoolean {
  JsonVariantType type;
  bool value;
};

// pointer to an allocated JsonObject
// *DOES NOT* store nullptr
struct JsonVariantContentObject {
  JsonVariantType type;
  JsonObject* pointer;
};

// pointer to an allocated JsonArray
// *DOES NOT* store nullptr
struct JsonVariantContentArray {
  JsonVariantType type;
  JsonArray* pointer;
};

// float, double
struct JsonVariantContentFloat {
  JsonVariantType type;
  JsonFloat value;
};

// char, short, int, long, long long
struct JsonVariantContentInteger {
  JsonVariantType type;
  JsonInteger value;
};

// -//- as above, but for unsigned types
struct JsonVariantContentUnsignedInteger {
  JsonVariantType type;
  JsonUnsignedInteger value;
};

// pointer to an allocated string (either parsed or unparsed)
// *COULD* store nullptr
struct JsonVariantContentString {
  JsonVariantType type;
  bool parsed;
  const char* pointer;
};

// The enum JsonVariantType determines which member is in use.
// Take care and only access the active union member data fields other than 'type'.
union JsonVariantContent {
  JsonVariantContent() noexcept :
    undefined({JsonVariantType::JSON_UNDEFINED})
  {}

  explicit JsonVariantContent(JsonNull) noexcept :
    null({JsonVariantType::JSON_NULL})
  {}

  explicit JsonVariantContent(bool value) noexcept :
    asBoolean({JsonVariantType::JSON_BOOLEAN, value})
  {}

  explicit JsonVariantContent(JsonObject* pointer) noexcept :
    asObject({JsonVariantType::JSON_OBJECT, pointer})
  {}

  explicit JsonVariantContent(JsonArray* pointer) noexcept :
    asArray({JsonVariantType::JSON_ARRAY, pointer})
  {}

  explicit JsonVariantContent(JsonFloat value) noexcept :
    asFloat({JsonVariantType::JSON_FLOAT, value})
  {}

  explicit JsonVariantContent(JsonInteger value) noexcept :
    asSignedInteger({JsonVariantType::JSON_SIGNED_INTEGER, value})
  {}

  explicit JsonVariantContent(JsonUnsignedInteger value) noexcept :
    asUnsignedInteger({JsonVariantType::JSON_UNSIGNED_INTEGER, value})
  {}

  explicit JsonVariantContent(const char* pointer, bool parsed = true) noexcept :
    asString({JsonVariantType::JSON_STRING, parsed, pointer})
  {}

  JsonVariantContentUndefined undefined;
  JsonVariantContentNull null;
  JsonVariantContentBoolean asBoolean;
  JsonVariantContentObject asObject;
  JsonVariantContentArray asArray;
  JsonVariantContentFloat asFloat;
  JsonVariantContentInteger asSignedInteger;
  JsonVariantContentUnsignedInteger asUnsignedInteger;
  JsonVariantContentString asString;
};

}  // namespace Internals
}  // namespace ArduinoJson
