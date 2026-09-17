// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson/Numbers/isNumber.hpp>
#include <catch.hpp>

#include "parseNumber.ipp"

TEST_CASE("isNumber() integral") {
  SECTION("Empty String") {
    REQUIRE_FALSE_IS_NUMBER("");
  }

  SECTION("Sign w/o a number") {
    REQUIRE_FALSE_IS_NUMBER("-");
    REQUIRE_FALSE_IS_NUMBER("+");
  }

  SECTION("Integer") {
    REQUIRE_IS_NUMBER("0");
    REQUIRE_IS_NUMBER("42");
    REQUIRE_IS_NUMBER("12345");
  }

  SECTION("Spaces") {
    REQUIRE_FALSE_IS_NUMBER("42  ");
    REQUIRE_FALSE_IS_NUMBER("  42");
    REQUIRE_FALSE_IS_NUMBER(" 42 ");
  }

  SECTION("Leading zeroes") {
    REQUIRE_IS_NUMBER("042");
    REQUIRE_IS_NUMBER("-042");
    REQUIRE_IS_NUMBER("+042");
  }

  SECTION("Signed integer") {
    REQUIRE_IS_NUMBER("-42");
    REQUIRE_IS_NUMBER("+42");
  }

  SECTION("More than one sign character") {
    REQUIRE_FALSE_IS_NUMBER("--42");
    REQUIRE_FALSE_IS_NUMBER("++42");
    REQUIRE_FALSE_IS_NUMBER("-+42");
    REQUIRE_FALSE_IS_NUMBER("+-42");
    REQUIRE_FALSE_IS_NUMBER("+42-");
    REQUIRE_FALSE_IS_NUMBER("-42+");
  }

  SECTION("Trailing sign") {
    REQUIRE_FALSE_IS_NUMBER("42-");
    REQUIRE_FALSE_IS_NUMBER("42+");
    REQUIRE_FALSE_IS_NUMBER("42-");
    REQUIRE_FALSE_IS_NUMBER("42+");
  }
}
