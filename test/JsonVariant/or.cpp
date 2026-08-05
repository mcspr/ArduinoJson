// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

static const JsonVariant undefined;
static const JsonVariant null = static_cast<const char*>(0);

TEST_CASE("JsonVariant::operator||") {
  SECTION("undefined or const char*") {
    std::string result = undefined || "default";
    REQUIRE(result == "default");
  }

  SECTION("undefined or int") {
    int result = undefined or 42;
    REQUIRE(result == 42);
  }

  SECTION("undefined or bool") {
    bool result = undefined or true;
    REQUIRE(result == true);
  }

  SECTION("null or const char*") {
    std::string result = null or "default";
    REQUIRE(result == "default");
  }

  SECTION("null or int") {
    int result = null or 42;
    REQUIRE(result == 42);
  }

  SECTION("null or bool") {
    bool result = null or true;
    REQUIRE(result == true);
  }

  SECTION("int or const char*") {
    JsonVariant variant = 42;
    std::string result = variant or "default";
    REQUIRE(result == "default");
  }

  SECTION("int or int") {
    JsonVariant variant = 0;
    int result = variant or 666;
    REQUIRE(result == 0);
  }

  SECTION("double or int") {
    JsonVariant variant = 42.0;
    int result = variant or 666;
    REQUIRE(result == 42);
  }

  SECTION("bool or bool") {
    JsonVariant variant = false;
    bool result = variant or true;
    REQUIRE(result == false);
  }

  SECTION("int or bool") {
    JsonVariant variant = 0;
    bool result = variant or true;
    REQUIRE(result == true);
  }

  SECTION("const char* or const char*") {
    JsonVariant variant = "not default";
    std::string result = variant or "default";
    REQUIRE(result == "not default");
  }

  SECTION("const char* or int") {
    JsonVariant variant = "not default";
    int result = variant or 42;
    REQUIRE(result == 42);
  }
}
