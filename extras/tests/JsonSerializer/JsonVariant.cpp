// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>
#include <limits>

#include "Literals.hpp"

template <typename T>
std::string serialize(T value) {
  JsonDocument doc;
  doc.to<JsonVariant>().set(value);
  std::string output;
  serializeJson(doc, output);
  return output;
}

TEST_CASE("serializeJson(JsonVariant)") {
  SECTION("JsonVariant") {
    CHECK(serialize(JsonVariant()) == "null");
  }

  SECTION("const char*") {
    CHECK(serialize(static_cast<const char*>(0)) == "null");
    CHECK(serialize("hello") == "\"hello\"");
  }

  SECTION("std::string") {
    CHECK(serialize("hello"_s) == "\"hello\"");
    CHECK(serialize("hello \"world\""_s) == "\"hello \\\"world\\\"\"");
    CHECK(serialize("hello\\world"_s) == "\"hello\\\\world\"");
    CHECK(serialize("fifty/fifty"_s) == "\"fifty/fifty\"");
    CHECK(serialize("hello'world"_s) == "\"hello'world\"");
    CHECK(serialize("hello\bworld"_s) == "\"hello\\bworld\"");
    CHECK(serialize("hello\fworld"_s) == "\"hello\\fworld\"");
    CHECK(serialize("hello\nworld"_s) == "\"hello\\nworld\"");
    CHECK(serialize("hello\rworld"_s) == "\"hello\\rworld\"");
    CHECK(serialize("hello\tworld"_s) == "\"hello\\tworld\"");
    CHECK(serialize("hello\0world"_s) == "\"hello\\u0000world\"");
  }

  SECTION("SerializedValue<const char*>") {
    CHECK(serialize(serialized("[1,2]")) == "[1,2]");
  }

  SECTION("SerializedValue<std::string>") {
    CHECK(serialize(serialized("[1,2]"_s)) == "[1,2]");
  }

  SECTION("double") {
    CHECK(serialize(0.0) == "0");
    CHECK(serialize(3.1415927) == "3.1415927");
    CHECK(serialize(-3.1415927) == "-3.1415927");
    CHECK(serialize(1.7976931348623157E+308) == "1.797693135e308");
    CHECK(serialize(4.94065645841247e-324) == "4.940656458e-324");
  }

  SECTION("float") {
    REQUIRE(sizeof(float) == 4);
    CHECK(serialize(3.1415927f) == "3.141593");
    CHECK(serialize(-3.1415927f) == "-3.141593");
    CHECK(serialize(3.4E+38f) == "3.4e38");
    CHECK(serialize(1.17549435e-38f) == "1.175494e-38");
  }

  SECTION("int") {
    CHECK(serialize(0) == "0");
    CHECK(serialize(42) == "42");
    CHECK(serialize(-42) == "-42");
  }

  SECTION("unsigned long") {
    CHECK(serialize(4294967295UL) == "4294967295");
  }

  SECTION("bool") {
    CHECK(serialize(true) == "true");
    CHECK(serialize(false) == "false");
  }

#if ARDUINOJSON_USE_LONG_LONG
  SECTION("int64_t") {
    CHECK(serialize(-9223372036854775807 - 1) == "-9223372036854775808");
    CHECK(serialize(9223372036854775807) == "9223372036854775807");
  }

  SECTION("uint64_t") {
    CHECK(serialize(18446744073709551615U) == "18446744073709551615");
  }
#endif
}
