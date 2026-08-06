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
using ArduinoJson::Internals::IsSame;
using ArduinoJson::Internals::IsArray;
using ArduinoJson::Internals::IsPointer;
using ArduinoJson::Internals::IsConst;
using ArduinoJson::Internals::IsVariant;
using ArduinoJson::Internals::IsInstantiationOf;

using ArduinoJson::Internals::Conditional;

using ArduinoJson::Internals::JsonMutableArraySubscript;
using ArduinoJson::Internals::JsonConstArraySubscript;
using ArduinoJson::Internals::JsonMutableObjectSubscript;
using ArduinoJson::Internals::JsonConstObjectSubscript;
using ArduinoJson::Internals::JsonSubscriptBase;
using ArduinoJson::Internals::JsonVariantBase;

template <typename>
struct TemplateOne {
};

template <typename>
struct TemplateTwo {
};

TEST_CASE("TypeTraits") {
  SECTION("IsBaseOf") {
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

    STATIC_REQUIRE_FALSE(IsBaseOf<BaseTwo, BaseOne>::value);
    STATIC_REQUIRE_FALSE(IsBaseOf<BaseOne, BaseTwo>::value);
    STATIC_REQUIRE_FALSE(IsBaseOf<BaseOne, DerivedFromBaseTwo>::value);
    STATIC_REQUIRE(IsBaseOf<BaseOne, DerivedFromBaseOne>::value);
    STATIC_REQUIRE(IsBaseOf<BaseOne, DerivedFromDerived>::value);
    STATIC_REQUIRE(IsBaseOf<BaseTwo, DerivedFromBaseTwo>::value);
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

  SECTION("IsSame") {
    struct One {
    };

    struct Two {
    };

    STATIC_REQUIRE(IsSame<One, One>::value);
    STATIC_REQUIRE(IsSame<Two, Two>::value);
    STATIC_REQUIRE(!IsSame<One, Two>::value);
    STATIC_REQUIRE(!IsSame<Two, One>::value);
  }


  SECTION("IsPointer") {
    STATIC_REQUIRE_FALSE((IsPointer<char[]>::value));
    STATIC_REQUIRE_FALSE((IsPointer<const char[]>::value));
    STATIC_REQUIRE_FALSE((IsPointer<const char[10]>::value));
    STATIC_REQUIRE_FALSE((IsPointer<const char*&>::value));
    STATIC_REQUIRE((IsPointer<char*>::value));
    STATIC_REQUIRE((IsPointer<const char*>::value));
    STATIC_REQUIRE((IsPointer<const char* const>::value));
  }

  SECTION("IsArray") {
    STATIC_REQUIRE_FALSE((IsArray<JsonMutableObjectSubscript<const char[10]> >::value));
    STATIC_REQUIRE_FALSE((IsArray<JsonConstObjectSubscript<const char[10]> >::value));
    STATIC_REQUIRE_FALSE((IsArray<JsonMutableArraySubscript >::value));
    STATIC_REQUIRE_FALSE((IsArray<JsonConstArraySubscript >::value));
    STATIC_REQUIRE_FALSE((IsArray<std::string>::value));
    STATIC_REQUIRE_FALSE((IsArray<const char*>::value));
    STATIC_REQUIRE_FALSE((IsArray<const char*&>::value));
    STATIC_REQUIRE_FALSE((IsArray<const char(&)[10]>::value));
    STATIC_REQUIRE((IsArray<char[]>::value));
    STATIC_REQUIRE((IsArray<const char[]>::value));
    STATIC_REQUIRE((IsArray<char[10]>::value));
    STATIC_REQUIRE((IsArray<const char[10]>::value));
  }

  SECTION("IsVariant") {
    STATIC_REQUIRE_FALSE(IsVariant<std::string>::value);
    STATIC_REQUIRE(IsVariant<JsonMutableObjectSubscript<const char*> >::value);
    STATIC_REQUIRE(IsVariant<JsonConstObjectSubscript<std::string> >::value);
    STATIC_REQUIRE(IsVariant<JsonMutableArraySubscript >::value);
    STATIC_REQUIRE(IsVariant<JsonConstArraySubscript >::value);
    STATIC_REQUIRE(IsVariant<JsonVariant>::value);
  }

  SECTION("IsConst") {
    STATIC_REQUIRE_FALSE((IsConst<char>::value));
    STATIC_REQUIRE((IsConst<const std::string>::value));
    STATIC_REQUIRE((IsConst<const char>::value));
  }

  SECTION("IsInstantiationOf") {
    STATIC_REQUIRE(IsInstantiationOf<TemplateOne, TemplateOne<int> >::value);
    STATIC_REQUIRE_FALSE(IsInstantiationOf<TemplateOne, TemplateTwo<int> >::value);
    STATIC_REQUIRE(IsInstantiationOf<TemplateTwo, TemplateTwo<int> >::value);
    STATIC_REQUIRE_FALSE(IsInstantiationOf<TemplateTwo, TemplateOne<int> >::value);
  }

  SECTION("Conditional") {
    struct One {
    };

    struct Two {
    };

    STATIC_REQUIRE(IsSame<One, Conditional<true, One, Two> >::value);
    STATIC_REQUIRE(IsSame<Two, Conditional<false, One, Two> >::value);

    STATIC_REQUIRE(IsSame<One, Conditional<false, Two, One> >::value);
    STATIC_REQUIRE(IsSame<Two, Conditional<true, Two, One> >::value);
  }
}
