// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

static constexpr const char* const_char_nullptr = nullptr;

TEST_CASE("JsonObject::createNestedArray()") {
  DynamicJsonBuffer _jsonBuffer;
  JsonObject& _object = _jsonBuffer.createObject();

  SECTION("success() should return true if key is not nullptr") {
    JsonArray& arr = _object.createNestedArray("key");
    REQUIRE(arr.success() == true);
  }

  SECTION("success() should return false if key is nullptr") {
    JsonArray& arr = _object.createNestedArray(const_char_nullptr);
    REQUIRE(arr.success() == false);
  }
}
