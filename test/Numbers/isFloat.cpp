// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson/Numbers/isFloat.hpp>
#include <catch.hpp>

using ArduinoJson::Internals::isFloat;

TEST_CASE("isFloat()") {
  SECTION("NoExponent") {
    REQUIRE(isFloat("3.14"));
    REQUIRE(isFloat("-3.14"));
    REQUIRE(isFloat("+3.14"));
  }

  SECTION("IntegralPartMissing") {
    REQUIRE(isFloat(".14"));
    REQUIRE(isFloat("-.14"));
    REQUIRE(isFloat("+.14"));
  }

  SECTION("FractionalPartMissing") {
    REQUIRE(isFloat("3."));
    REQUIRE(isFloat("-3.e14"));
    REQUIRE(isFloat("+3.e-14"));
  }

  SECTION("NoDot") {
    REQUIRE(isFloat("3e14"));
    REQUIRE(isFloat("3e-14"));
    REQUIRE(isFloat("3e+14"));
  }

  SECTION("Integer") {
    REQUIRE(isFloat("14"));
    REQUIRE(isFloat("-14"));
    REQUIRE(isFloat("+14"));
  }

  SECTION("ExponentMissing") {
    REQUIRE_FALSE(isFloat("3.14e"));
    REQUIRE_FALSE(isFloat("3.14e-"));
    REQUIRE_FALSE(isFloat("3.14e+"));
  }

  SECTION("JustASign") {
    REQUIRE_FALSE(isFloat("-"));
    REQUIRE_FALSE(isFloat("+"));
  }

  SECTION("Empty") {
    REQUIRE_FALSE(isFloat(""));
  }

  SECTION("NaN") {
    REQUIRE_FALSE(isFloat("n"));
    REQUIRE_FALSE(isFloat("Na"));
    REQUIRE_FALSE(isFloat("NAN"));
    REQUIRE_FALSE(isFloat("nAn"));
    REQUIRE_FALSE(isFloat("nNaNn"));
    REQUIRE(isFloat("NaN"));
    REQUIRE(isFloat("nan"));
    REQUIRE(isFloat("-NaN"));
    REQUIRE(isFloat("+NaN"));
  }

  SECTION("Infinity") {
    REQUIRE(isFloat("inf"));
    REQUIRE(isFloat("-inf"));
    REQUIRE(isFloat("+inf"));
    REQUIRE(isFloat("Infinity"));
    REQUIRE(isFloat("+Infinity"));
    REQUIRE(isFloat("-Infinity"));
    REQUIRE_FALSE(isFloat("infinity"));
    REQUIRE_FALSE(isFloat("inF"));
    REQUIRE_FALSE(isFloat("infinityy"));
    REQUIRE_FALSE(isFloat("iinfinityy"));
  }
}
