// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

using StringBufferValue = ArduinoJson::Internals::JsonVariantContent::StringBufferValue;
using ArduinoJson::Internals::JsonString;

void checkIsArray(JsonVariant var) {
  REQUIRE(var.is<JsonArray>());
  REQUIRE(var.is<JsonArray&>());
  REQUIRE(var.is<const JsonArray>());
  REQUIRE(var.is<const JsonArray&>());

  REQUIRE_FALSE(var.is<bool>());
  REQUIRE_FALSE(var.is<double>());
  REQUIRE_FALSE(var.is<float>());
  REQUIRE_FALSE(var.is<int>());
  REQUIRE_FALSE(var.is<long>());
  REQUIRE_FALSE(var.is<const char*>());
  REQUIRE_FALSE(var.is<JsonObject>());
}

void checkIsBool(JsonVariant var) {
  REQUIRE(var.is<bool>());

  REQUIRE_FALSE(var.is<double>());
  REQUIRE_FALSE(var.is<float>());
  REQUIRE_FALSE(var.is<int>());
  REQUIRE_FALSE(var.is<long>());
  REQUIRE_FALSE(var.is<const char*>());
  REQUIRE_FALSE(var.is<JsonArray>());
  REQUIRE_FALSE(var.is<JsonObject>());
}

void checkIsStringBool(JsonVariant var) {
  REQUIRE(var.is<const char*>());
  REQUIRE(var.is<bool>());

  REQUIRE_FALSE(var.is<double>());
  REQUIRE_FALSE(var.is<float>());
  REQUIRE_FALSE(var.is<int>());
  REQUIRE_FALSE(var.is<long>());
  REQUIRE_FALSE(var.is<JsonArray>());
  REQUIRE_FALSE(var.is<JsonObject>());
}

void checkIsInteger(JsonVariant var) {
  REQUIRE(var.is<long>());
  REQUIRE(var.is<int>());
  REQUIRE(var.is<float>());
  REQUIRE(var.is<double>());

  REQUIRE_FALSE(var.is<bool>());
  REQUIRE_FALSE(var.is<const char*>());
  REQUIRE_FALSE(var.is<JsonArray>());
  REQUIRE_FALSE(var.is<JsonObject>());
}

void checkIsStringInteger(JsonVariant var) {
  REQUIRE(var.is<const char*>());
  REQUIRE(var.is<long>());
  REQUIRE(var.is<int>());
  REQUIRE(var.is<float>());
  REQUIRE(var.is<double>());

  REQUIRE_FALSE(var.is<bool>());
  REQUIRE_FALSE(var.is<JsonArray>());
  REQUIRE_FALSE(var.is<JsonObject>());
}

void checkIsFloat(JsonVariant var) {
  REQUIRE(var.is<double>());
  REQUIRE(var.is<float>());

  REQUIRE_FALSE(var.is<bool>());
  REQUIRE_FALSE(var.is<int>());
  REQUIRE_FALSE(var.is<long>());
  REQUIRE_FALSE(var.is<const char*>());
  REQUIRE_FALSE(var.is<JsonArray>());
  REQUIRE_FALSE(var.is<JsonObject>());
}

void checkIsStringFloat(JsonVariant var) {
  REQUIRE(var.is<const char*>());
  REQUIRE(var.is<double>());
  REQUIRE(var.is<float>());

  REQUIRE_FALSE(var.is<bool>());
  REQUIRE_FALSE(var.is<int>());
  REQUIRE_FALSE(var.is<long>());
  REQUIRE_FALSE(var.is<JsonArray>());
  REQUIRE_FALSE(var.is<JsonObject>());
}

void checkIsNull(JsonVariant var) {
  REQUIRE(var.is<JsonNull>());

  REQUIRE_FALSE(var.is<bool>());
  REQUIRE_FALSE(var.is<int>());
  REQUIRE_FALSE(var.is<long>());
  REQUIRE_FALSE(var.is<const char*>());
  REQUIRE_FALSE(var.is<float>());
  REQUIRE_FALSE(var.is<double>());
  REQUIRE_FALSE(var.is<JsonArray>());
  REQUIRE_FALSE(var.is<JsonObject>());
}

void checkIsStringNull(JsonVariant var) {
  REQUIRE(var.is<const char*>());
  REQUIRE(var.is<JsonNull>());

  REQUIRE_FALSE(var.is<bool>());
  REQUIRE_FALSE(var.is<int>());
  REQUIRE_FALSE(var.is<long>());
  REQUIRE_FALSE(var.is<float>());
  REQUIRE_FALSE(var.is<double>());
  REQUIRE_FALSE(var.is<JsonArray>());
  REQUIRE_FALSE(var.is<JsonObject>());
}

void checkIsString(JsonVariant var) {
  REQUIRE(var.is<const char*>());
  REQUIRE(var.is<std::string>());

  REQUIRE_FALSE(var.is<bool>());
  REQUIRE_FALSE(var.is<int>());
  REQUIRE_FALSE(var.is<double>());
  REQUIRE_FALSE(var.is<float>());
  REQUIRE_FALSE(var.is<long>());
  REQUIRE_FALSE(var.is<JsonArray>());
  REQUIRE_FALSE(var.is<JsonObject>());
}

TEST_CASE("JsonVariant::is()") {
  DynamicJsonBuffer jsonBuffer;

  SECTION("JsonArray") {
    checkIsArray(jsonBuffer.createArray());
  }

  SECTION("null") {
    checkIsNull(JsonNull{});
  }

  SECTION("bool") {
    checkIsBool(true);
    checkIsBool(false);
  }

  SECTION("double") {
    checkIsFloat(4.2);
  }

  SECTION("int") {
    checkIsInteger(42);
  }

  SECTION("long") {
    checkIsInteger(42L);
  }

  SECTION("string") {
    checkIsString("42");
    checkIsString(StringBufferValue{"42"});
  }

  SECTION("string to string conversion") {
    checkIsString(JsonVariant(
      JsonString("42"), true));
    checkIsString(JsonVariant(
      JsonString(StringBufferValue{"42"}), true));
    checkIsString(JsonVariant(
      JsonString("wat"), false));
    checkIsString(JsonVariant(
      JsonString(StringBufferValue{"wat"}), false));
  }

  SECTION("unparsed null") {
    checkIsString(RawJson("null"));
    checkIsStringNull(RawJson("null"));
  }

  SECTION("unparsed bool") {
    checkIsStringBool(RawJson("true"));
    checkIsStringBool(RawJson("false"));
  }

  SECTION("unparsed int") {
    checkIsStringInteger(RawJson("42"));
  }

  SECTION("unparsed float") {
    checkIsStringFloat(RawJson("4.2e-10"));
  }
}
