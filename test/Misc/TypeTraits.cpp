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

using ArduinoJson::Internals::JsonMutableArraySubscript;
using ArduinoJson::Internals::JsonConstArraySubscript;
using ArduinoJson::Internals::JsonMutableObjectSubscript;
using ArduinoJson::Internals::JsonConstObjectSubscript;
using ArduinoJson::Internals::JsonSubscriptBase;
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
  }

  SECTION("Recursive IsBaseOf") {
    STATIC_REQUIRE(IsBaseOf<JsonVariantBase<JsonVariant>, JsonVariant>::value);

    using mutable_object_subscript_type = JsonMutableObjectSubscript<const char*>;
    using mutable_object_subscript_base = JsonSubscriptBase<JsonObject, mutable_object_subscript_type>;
    STATIC_REQUIRE(IsBaseOf<mutable_object_subscript_base, mutable_object_subscript_type>::value);
    STATIC_REQUIRE(!IsBaseOf<JsonConstObjectSubscript<const char*>, mutable_object_subscript_type>::value);

    using const_object_subscript_type = JsonConstObjectSubscript<const char*>;
    using const_object_subscript_base = JsonSubscriptBase<const JsonObject, const_object_subscript_type>;
    STATIC_REQUIRE(IsBaseOf<const_object_subscript_base, const_object_subscript_type>::value);
    STATIC_REQUIRE(!IsBaseOf<JsonMutableObjectSubscript<const char*>, const_object_subscript_type>::value);

    using mutable_array_subscript_type = JsonMutableArraySubscript;
    using mutable_array_subscript_base = JsonSubscriptBase<JsonArray, mutable_array_subscript_type>;

    STATIC_REQUIRE(IsBaseOf<mutable_array_subscript_base, mutable_array_subscript_type>::value);
    STATIC_REQUIRE(!IsBaseOf<JsonConstArraySubscript, mutable_array_subscript_type>::value);

    using const_array_subscript_type = JsonConstArraySubscript;
    using const_array_subscript_base = JsonSubscriptBase<const JsonArray, const_array_subscript_type>;

    STATIC_REQUIRE(IsBaseOf<const_array_subscript_base, const_array_subscript_type>::value);
    STATIC_REQUIRE(!IsBaseOf<JsonMutableArraySubscript, const_array_subscript_type>::value);
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
    REQUIRE_FALSE((IsArray<JsonMutableObjectSubscript<const char[10]> >::value));
    REQUIRE_FALSE((IsArray<JsonConstObjectSubscript<const char[10]> >::value));
    REQUIRE_FALSE((IsArray<JsonMutableArraySubscript >::value));
    REQUIRE_FALSE((IsArray<JsonConstArraySubscript >::value));
    REQUIRE_FALSE((IsArray<std::string>::value));
    REQUIRE_FALSE((IsArray<const char*>::value));
    REQUIRE_FALSE((IsArray<const char*&>::value));
    REQUIRE_FALSE((IsArray<const char(&)[10]>::value));
    REQUIRE((IsArray<const char[]>::value));
    REQUIRE((IsArray<const char[10]>::value));
  }

  SECTION("IsVariant") {
    REQUIRE_FALSE(IsVariant<std::string>::value);
    REQUIRE(IsVariant<JsonMutableObjectSubscript<const char*> >::value);
    REQUIRE(IsVariant<JsonConstObjectSubscript<std::string> >::value);
    REQUIRE(IsVariant<JsonMutableArraySubscript >::value);
    REQUIRE(IsVariant<JsonConstArraySubscript >::value);
    REQUIRE(IsVariant<JsonVariant>::value);
  }

  SECTION("IsConst") {
    REQUIRE_FALSE((IsConst<char>::value));
    REQUIRE((IsConst<const std::string>::value));
    REQUIRE((IsConst<const char>::value));
  }
}
