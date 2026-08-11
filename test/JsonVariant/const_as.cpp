// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonVariant::as<const T&>()") {
  DynamicJsonBuffer _jsonBuffer;

  SECTION("Array reference") {
    JsonArray &arr = _jsonBuffer.createArray();
    arr.add("element at index 0");
    arr.add("element at index 1");

    JsonVariant var = const_cast<const JsonArray&>(arr);
    REQUIRE_FALSE(var.is<JsonArray&>());
    REQUIRE(var.is<const JsonArray&>());

    auto& invalid = var.as<JsonArray&>();
    REQUIRE_FALSE(invalid.success());

    auto& ref = var.as<const JsonArray&>();
    REQUIRE(2 == ref.size());
    REQUIRE(ref[0].success());
    REQUIRE(ref[1].success());
    REQUIRE_FALSE(ref[3].success());
    REQUIRE(std::string("element at index 0") == ref[0]);
    REQUIRE(std::string("element at index 1") == ref[1]);
  }

  SECTION("Object reference") {
    JsonObject &obj = _jsonBuffer.createObject();
    obj["a"] = "element at key \"a\"";
    obj["b"] = "element at key \"b\"";

    JsonVariant var = const_cast<const JsonObject&>(obj);
    REQUIRE_FALSE(var.is<JsonObject&>());
    REQUIRE(var.is<const JsonObject&>());

    auto& invalid = var.as<JsonObject&>();
    REQUIRE_FALSE(invalid.success());

    auto& ref = var.as<const JsonObject&>();

    REQUIRE(2 == ref.size());
    REQUIRE(ref["a"].success());
    REQUIRE(ref["b"].success());
    REQUIRE_FALSE(ref["c"].success());
    REQUIRE(std::string("element at key \"a\"") == ref["a"]);
    REQUIRE(std::string("element at key \"b\"") == ref["b"]);
  }
}
