// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#include <catch.hpp>
#include <limits>
#include <string>

#define ARDUINOJSON_ENABLE_NAN 1
#define ARDUINOJSON_ENABLE_INFINITY 1
#include <ArduinoJson/Json/TextFormatter.hpp>
#include <ArduinoJson/Serialization/Writer.hpp>

using namespace ArduinoJson::detail;

template <typename TFloat>
static std::string toString(TFloat input) {
  std::string output;
  Writer<std::string> sb(output);
  TextFormatter<Writer<std::string>> writer(sb);
  writer.writeFloat(input);
  return output;
}

TEST_CASE("TextFormatter::writeFloat(double)") {
  SECTION("Pi") {
    REQUIRE(toString(3.14159265359) == "3.14159265");
  }

  SECTION("Signaling NaN") {
    double nan = std::numeric_limits<double>::signaling_NaN();
    REQUIRE(toString(nan) == "NaN");
  }

  SECTION("Quiet NaN") {
    double nan = std::numeric_limits<double>::quiet_NaN();
    REQUIRE(toString(nan) == "NaN");
  }

  SECTION("Infinity") {
    double inf = std::numeric_limits<double>::infinity();
    REQUIRE(toString(inf) == "Infinity");
    REQUIRE(toString(-inf) == "-Infinity");
  }

  SECTION("Zero") {
    REQUIRE(toString(0.0) == "0");
    REQUIRE(toString(-0.0) == "0");
  }

  SECTION("Espilon") {
    REQUIRE(toString(2.2250738585072014E-308) == "2.22507386e-308");
    REQUIRE(toString(-2.2250738585072014E-308) == "-2.22507386e-308");
  }

  SECTION("Max double") {
    REQUIRE(toString(1.7976931348623157E+308) == "1.79769313e308");
    REQUIRE(toString(-1.7976931348623157E+308) == "-1.79769313e308");
  }

  SECTION("Big exponent") {
    REQUIRE(toString(1e255) == "1e255");
    REQUIRE(toString(1e-255) == "1e-255");
  }

  SECTION("Exponentation when <= 1e-5") {
    REQUIRE(toString(1e-4) == "0.0001");
    REQUIRE(toString(1e-5) == "1e-5");

    REQUIRE(toString(-1e-4) == "-0.0001");
    REQUIRE(toString(-1e-5) == "-1e-5");
  }

  SECTION("Exponentation when >= 1e7") {
    REQUIRE(toString(9999999.99) == "9999999.99");
    REQUIRE(toString(10000000.0) == "1e7");

    REQUIRE(toString(-9999999.99) == "-9999999.99");
    REQUIRE(toString(-10000000.0) == "-1e7");
  }

  SECTION("Rounding when too many decimals") {
    REQUIRE(toString(0.000099999999999) == "0.0001");
    REQUIRE(toString(0.0000099999999999) == "1e-5");
    REQUIRE(toString(0.9999999996) == "1");
  }

  SECTION("9 decimal places") {
    REQUIRE(toString(0.10000001) == "0.10000001");
    REQUIRE(toString(0.99999999) == "0.99999999");

    REQUIRE(toString(9.00000001) == "9.00000001");
    REQUIRE(toString(9.99999999) == "9.99999999");
  }

  SECTION("9 decimal places") {
    REQUIRE(toString(0.100000001) == "0.100000001");
    REQUIRE(toString(0.999999999) == "0.999999999");

    REQUIRE(toString(9.000000001) == "9");
    REQUIRE(toString(9.999999999) == "10");
  }

  SECTION("10 decimal places") {
    REQUIRE(toString(0.1000000001) == "0.1");
    REQUIRE(toString(0.9999999999) == "1");

    REQUIRE(toString(9.0000000001) == "9");
    REQUIRE(toString(9.9999999999) == "10");
  }
}

TEST_CASE("TextFormatter::writeFloat(float)") {
  SECTION("Pi") {
    REQUIRE(toString(3.14159265359f) == "3.141593");
  }

  SECTION("999.9") {  // issue #543
    REQUIRE(toString(999.9f) == "999.9");
  }

  SECTION("24.3") {  // # issue #588
    REQUIRE(toString(24.3f) == "24.3");
  }
}
