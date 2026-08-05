// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

static constexpr const char* const_char_nullptr = nullptr;

TEST_CASE("JsonObject::createNestedObject()") {
  DynamicJsonBuffer _jsonBuffer;
  JsonObject& _object = _jsonBuffer.createObject();

  SECTION("success() should return true if key is not nullptr") {
    JsonObject& obj = _object.createNestedObject("key");
    REQUIRE(obj.success());
  }

  SECTION("success() should return false if key is nullptr") {
    JsonObject& obj = _object.createNestedObject(const_char_nullptr);
    REQUIRE_FALSE(obj.success());
  }

  SECTION("success() returns false when allocation fails") {
    StaticJsonBuffer<JSON_OBJECT_SIZE(1)> jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    REQUIRE(obj.success());
    JsonObject& other = obj.createNestedObject("hello");
    REQUIRE_FALSE(other.success());
  }
}
