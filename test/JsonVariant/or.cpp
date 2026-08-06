// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonVariant::operator||") {
  JsonVariant variant;

  SECTION("undefined or const char*") {
    std::string result = variant or "default";
    REQUIRE(result == "default");
  }

  SECTION("undefined or int") {
    int result = variant or 42;
    REQUIRE(result == 42);
  }

  SECTION("undefined or bool") {
    bool result = variant or true;
    REQUIRE(result == true);
  }

  SECTION("null or const char*") {
    variant = JsonNull{};
    std::string result = variant or "default";
    REQUIRE(result == "default");
  }

  SECTION("null or int") {
    variant = JsonNull{};
    int result = variant or 42;
    REQUIRE(result == 42);
  }

  SECTION("null or bool") {
    variant = JsonNull{};
    bool result = variant or true;
    REQUIRE(result == true);
  }

  SECTION("int or const char*") {
    variant = 42;
    std::string result = variant or "default";
    REQUIRE(result == "default");
  }

  SECTION("int or int") {
    variant = 0;
    int result = variant or 666;
    REQUIRE(result == 0);
  }

  SECTION("double or int") {
    variant = 42.0;
    int result = variant or 666;
    REQUIRE(result == 42);
  }

  SECTION("bool or bool") {
    variant = false;
    bool result = variant or true;
    REQUIRE(result == false);
  }

  SECTION("int or bool") {
    variant = 0;
    bool result = variant or true;
    REQUIRE(result == true);
  }

  SECTION("const char* or const char* w/ valid pointer") {
    variant = "not default";
    std::string result = variant or "default";
    REQUIRE(result == "not default");
  }

  SECTION("const char* or const char* w/ nullptr") {
    variant = static_cast<const char *>(nullptr);
    std::string result = variant or "default";
    REQUIRE(result == "default");
  }

  SECTION("const char* or int") {
    variant = "not default";
    REQUIRE((variant or 42) == 42);
  }

  SECTION("JsonObject or JsonObject") {
    JsonObject& invalid = JsonObject::invalid();
    REQUIRE(std::addressof(variant or invalid) ==
            std::addressof(JsonObject::invalid()));

    StaticJsonBuffer <JSON_OBJECT_SIZE(1)> jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    REQUIRE(std::addressof(variant or obj) ==
            std::addressof(obj));
  }

  SECTION("JsonArray or JsonArray") {
    JsonArray& invalid = JsonArray::invalid();
    REQUIRE(std::addressof(variant or invalid) ==
            std::addressof(invalid));

    StaticJsonBuffer <JSON_OBJECT_SIZE(1)> jsonBuffer;
    JsonArray& arr = jsonBuffer.createArray();
    REQUIRE(std::addressof(variant or arr) ==
            std::addressof(arr));
  }

  SECTION("JsonVariant or JsonVariant") {
    const JsonVariant defaultValue = "default";
    REQUIRE((variant or defaultValue) == "default");
    REQUIRE((defaultValue or variant) == "default");

    variant = static_cast<const char *>(nullptr);
    REQUIRE((variant or defaultValue) == "default");
    REQUIRE((defaultValue or variant) == "default");
  }
}
