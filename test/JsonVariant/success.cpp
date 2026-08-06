// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonVariant::success()") {
  SECTION("ReturnsFalse_WhenUndefined") {
    JsonVariant variant;
    REQUIRE_FALSE(variant.success());
  }

  SECTION("ReturnsTrue_WhenInteger") {
    JsonVariant variant = 0;
    REQUIRE(variant.success());
  }

  SECTION("ReturnsTrue_WhenString") {
    JsonVariant variant = "hello";
    REQUIRE(variant.success());
  }

  SECTION("ReturnsTrue_WhenStringNullptr") {
    JsonVariant variant = static_cast<const char *>(nullptr);
    REQUIRE_FALSE(variant.success());
  }

  SECTION("ReturnsTrue_WhenEmptyArray") {
    DynamicJsonBuffer jsonBuffer;

    JsonVariant variant = jsonBuffer.createArray();
    REQUIRE(variant.success());
  }

  SECTION("ReturnsTrue_WhenEmptyObject") {
    DynamicJsonBuffer jsonBuffer;

    JsonVariant variant = jsonBuffer.createObject();
    REQUIRE(variant.success());
  }

  SECTION("ReturnsFalse_WhenInvalidArray") {
    JsonVariant variant = JsonArray::invalid();
    REQUIRE_FALSE(variant.success());
  }

  SECTION("ReturnsFalse_WhenInvalidObject") {
    JsonVariant variant = JsonObject::invalid();
    REQUIRE_FALSE(variant.success());
  }
}
