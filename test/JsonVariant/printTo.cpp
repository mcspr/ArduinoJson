// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

#include <limits>
#include <string>

void check(JsonVariant variant, const std::string &expected) {
  char buffer[256] = "";
  size_t returnValue = variant.printTo(buffer, sizeof(buffer));
  REQUIRE(expected == std::string(buffer, returnValue));
  REQUIRE(expected.size() == returnValue);
}

TEST_CASE("JsonVariant::printTo()") {
  SECTION("Empty") {
    check(JsonVariant(), "null");
  }

  SECTION("Null") {
    check(JsonNull{}, "null");
  }

  SECTION("Raw string") {
    check(RawJson("whatever"), "whatever");
  }

  SECTION("Null string") {
    check(static_cast<const char*>(nullptr), "\"\"");
  }

  SECTION("Empty string") {
    check("", "\"\"");
  }

  SECTION("String") {
    check("hello", "\"hello\"");
  }

  SECTION("Double") {
    check(3.1415927, "3.1415927");
  }

  SECTION("Integer") {
    check(42, "42");
  }

  SECTION("NegativeLong") {
    check(-42, "-42");
  }

  SECTION("UnsignedLong") {
    check(4294967295UL, "4294967295");
  }

  SECTION("True") {
    check(true, "true");
  }

  SECTION("OneFalse") {
    check(false, "false");
  }

#if ARDUINOJSON_USE_LONG_LONG || ARDUINOJSON_USE_INT64
  SECTION("NegativeInt64") {
    constexpr auto value = std::numeric_limits<signed long long>::min();
    check(value, std::to_string(value));
  }

  SECTION("PositiveInt64") {
    constexpr auto value = std::numeric_limits<signed long long>::max();
    check(value, std::to_string(value));
  }

  SECTION("UInt64 min") {
    constexpr auto value = std::numeric_limits<unsigned long long>::min();
    check(value, std::to_string(value));
  }

  SECTION("UInt64 max") {
    constexpr auto value = std::numeric_limits<unsigned long long>::max();
    check(value, std::to_string(value));
  }
#endif
}
