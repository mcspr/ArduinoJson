// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

#include <ArduinoJson/TypeTraits/IsBaseOf.hpp>
#include <ArduinoJson/TypeTraits/IsArray.hpp>
#include <ArduinoJson/TypeTraits/IsPointer.hpp>
#include <ArduinoJson/TypeTraits/IsConst.hpp>
#include <ArduinoJson/TypeTraits/IsVariant.hpp>

using ArduinoJson::Internals::IsBaseOf;
using ArduinoJson::Internals::IsArray;
using ArduinoJson::Internals::IsPointer;
using ArduinoJson::Internals::IsConst;
using ArduinoJson::Internals::IsVariant;

using ArduinoJson::Internals::JsonObjectSubscript;
using ArduinoJson::Internals::JsonVariantBase;

struct BaseOne {
};

struct DerivedFromBaseOne : public BaseOne {
};

struct DerivedFromDerived : public DerivedFromBaseOne {
};

struct BaseTwo {
};


struct DerivedFromBaseTwo : public BaseTwo {
};

TEST_CASE("TypeTraits") {
  SECTION("IsBaseOf") {
    REQUIRE_FALSE(IsBaseOf<BaseTwo, BaseOne>::value);
    REQUIRE_FALSE(IsBaseOf<BaseOne, BaseTwo>::value);
    REQUIRE_FALSE(IsBaseOf<BaseOne, DerivedFromBaseTwo>::value);
    REQUIRE(IsBaseOf<BaseOne, DerivedFromBaseOne>::value);
    REQUIRE(IsBaseOf<BaseOne, DerivedFromDerived>::value);
    REQUIRE(IsBaseOf<BaseTwo, DerivedFromBaseTwo>::value);
    REQUIRE(IsBaseOf<
      JsonVariantBase<JsonObjectSubscript<const char*> >,
      JsonObjectSubscript<const char*> >::value);
    REQUIRE(IsBaseOf<
      JsonVariantBase<JsonObjectSubscript<std::string> >,
      JsonObjectSubscript<std::string> >::value);
  }

  SECTION("IsPointer") {
    REQUIRE_FALSE((IsPointer<char[]>::value));
    REQUIRE_FALSE((IsPointer<const char[]>::value));
    REQUIRE_FALSE((IsPointer<const char[10]>::value));
    REQUIRE_FALSE((IsPointer<const char*&>::value));
    REQUIRE((IsPointer<char*>::value));
    REQUIRE((IsPointer<const char*>::value));
    REQUIRE((IsPointer<const char* const>::value));
  }

  SECTION("IsArray") {
    REQUIRE_FALSE((IsArray<JsonObjectSubscript<const char[10]> >::value));
    REQUIRE_FALSE((IsArray<std::string>::value));
    REQUIRE_FALSE((IsArray<const char*>::value));
    REQUIRE_FALSE((IsArray<const char*&>::value));
    REQUIRE_FALSE((IsArray<const char(&)[10]>::value));
    REQUIRE((IsArray<const char[]>::value));
    REQUIRE((IsArray<const char[10]>::value));
  }

  SECTION("IsVariant") {
    REQUIRE_FALSE(IsVariant<std::string>::value);
    REQUIRE(IsVariant<JsonObjectSubscript<const char*> >::value);
    REQUIRE(IsVariant<JsonObjectSubscript<std::string> >::value);
    REQUIRE(IsVariant<JsonVariant>::value);
  }

  SECTION("IsConst") {
    REQUIRE_FALSE((IsConst<char>::value));
    REQUIRE((IsConst<const std::string>::value));
    REQUIRE((IsConst<const char>::value));
  }
}
