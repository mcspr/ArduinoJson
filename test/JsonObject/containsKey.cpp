// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonObject::containsKey()") {
  DynamicJsonBuffer _jsonBuffer;
  JsonObject& _object = _jsonBuffer.createObject();

  SECTION("ContainsKeyReturnsFalseForNonExistingKey") {
    REQUIRE_FALSE(_object.containsKey("hello"));
    REQUIRE_FALSE(_object.containsKey("world"));
    REQUIRE(_object.set("hello", 42));
    REQUIRE(_object.containsKey("hello"));
    REQUIRE_FALSE(_object.containsKey("world"));
  }

  SECTION("ContainsKeyReturnsTrueForDefinedValue") {
    REQUIRE_FALSE(_object.containsKey("hello"));
    REQUIRE(_object.set("hello", 42));
    REQUIRE(_object.containsKey("hello"));
  }

  SECTION("ContainsKeyReturnsFalseAfterRemove") {
    REQUIRE(_object.set("hello", 42));
    REQUIRE(_object.containsKey("hello"));
    _object.remove("hello");

    REQUIRE_FALSE(_object.containsKey("hello"));
  }
}
