// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson/Numbers/isNumber.hpp>
#include <catch.hpp>

#include "parseNumber.ipp"

TEST_CASE("isNumber() floating point") {
  SECTION("No exponent") {
    REQUIRE_IS_NUMBER("3.14");
    REQUIRE_IS_NUMBER("-3.14");
    REQUIRE_IS_NUMBER("+3.14");
  }

  SECTION("Integral part missing") {
    REQUIRE_IS_NUMBER(".14");
    REQUIRE_IS_NUMBER("-.14");
    REQUIRE_IS_NUMBER("+.14");
    REQUIRE_FALSE_IS_NUMBER(". 14");
    REQUIRE_FALSE_IS_NUMBER(" . 14");
    REQUIRE_FALSE_IS_NUMBER(" .14");
    REQUIRE_FALSE_IS_NUMBER(".14.");
    REQUIRE_FALSE_IS_NUMBER("-. 14");
    REQUIRE_FALSE_IS_NUMBER(" -. 14");
    REQUIRE_FALSE_IS_NUMBER(" -.14.");
    REQUIRE_FALSE_IS_NUMBER("+. 14");
    REQUIRE_FALSE_IS_NUMBER(" +. 14");
    REQUIRE_FALSE_IS_NUMBER(" +.14.");
  }

  SECTION("Fractional part missing") {
    REQUIRE_IS_NUMBER("3.");
    REQUIRE_IS_NUMBER("-3.e14");
    REQUIRE_IS_NUMBER("+3.e-14");
  }

  SECTION("No decimal point") {
    REQUIRE_IS_NUMBER("3e14");
    REQUIRE_IS_NUMBER("3e-14");
    REQUIRE_IS_NUMBER("3e+14");
  }

  SECTION("No integral or decimal") {
    REQUIRE_FALSE_IS_NUMBER(".");
    REQUIRE_FALSE_IS_NUMBER("+.");
    REQUIRE_FALSE_IS_NUMBER(" .");
    REQUIRE_FALSE_IS_NUMBER(" . ");
    REQUIRE_FALSE_IS_NUMBER("  .");
  }

  SECTION("Exponent w/o integral part") {
    REQUIRE_FALSE_IS_NUMBER("e123");
    REQUIRE_FALSE_IS_NUMBER("+e123");
    REQUIRE_FALSE_IS_NUMBER("-e123");
  }

  SECTION("Exponent value missing") {
    REQUIRE_FALSE_IS_NUMBER("3.14e");
    REQUIRE_FALSE_IS_NUMBER("3.14e-");
    REQUIRE_FALSE_IS_NUMBER("3.14e+");
  }

  SECTION("Exponent leading zeros") {
    REQUIRE_IS_NUMBER("0.3140e1");
    REQUIRE_IS_NUMBER("0.314e02");
    REQUIRE_IS_NUMBER("0.314e003");
    REQUIRE_IS_NUMBER("0.3140e004");
  }

  SECTION("NaN") {
    REQUIRE_FALSE_IS_NUMBER("n");
    REQUIRE_FALSE_IS_NUMBER("Na");
    REQUIRE_FALSE_IS_NUMBER("NAN");
    REQUIRE_FALSE_IS_NUMBER("nAn");
    REQUIRE_FALSE_IS_NUMBER("nNaNn");
    REQUIRE_IS_NUMBER("NaN");
    REQUIRE_IS_NUMBER("nan");
    REQUIRE_IS_NUMBER("-NaN");
    REQUIRE_IS_NUMBER("+NaN");
  }

  SECTION("Infinity") {
    REQUIRE_IS_NUMBER("inf");
    REQUIRE_IS_NUMBER("-inf");
    REQUIRE_IS_NUMBER("+inf");
    REQUIRE_IS_NUMBER("Infinity");
    REQUIRE_IS_NUMBER("+Infinity");
    REQUIRE_IS_NUMBER("-Infinity");
    REQUIRE_FALSE_IS_NUMBER("infinity");
    REQUIRE_FALSE_IS_NUMBER("inF");
    REQUIRE_FALSE_IS_NUMBER("infinityy");
    REQUIRE_FALSE_IS_NUMBER("iinfinityy");
  }
}
