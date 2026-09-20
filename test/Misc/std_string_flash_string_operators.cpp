// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#define ARDUINOJSON_CHAR_POINTER_IS_PROGMEM 1
#define ARDUINOJSON_ENABLE_PROGMEM 1

#include <ArduinoJson.h>
#include <catch.hpp>
#include <string>

#define ARDUINOJSON_STD_STRING_TEST_CASE "std::string w/ Strings::FlashString instead of Strings::CharPointer"
#include "std_string.ipp"

namespace std_string_traits_test_cases {
namespace {

struct Foo {
  Foo(const __FlashStringHelper*);
};

struct Bar;

using ArduinoJson::Internals::Strings::FlashString::Detail::CanConstruct;
using ArduinoJson::Internals::Strings::FlashString::Probe;

TEST_CASE("FlashString detail") {
  SECTION("No-op Probe") {
    STATIC_REQUIRE(Probe(nullptr));
  }

  SECTION("CanConstruct") {
    STATIC_REQUIRE(CanConstruct<Foo>::value);
    STATIC_REQUIRE_FALSE(CanConstruct<Bar>::value);
    STATIC_REQUIRE_FALSE(CanConstruct<std::string>::value);
  }
}

}  // namespace
}  // namespace std_string_traits_test_cases
