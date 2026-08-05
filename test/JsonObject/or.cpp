// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonObject subscript ::operator||") {
  JsonObject& invalid = JsonObject::invalid();

  StaticJsonBuffer<JSON_OBJECT_SIZE(1)> jsonBuffer;
  JsonObject& obj = jsonBuffer.createObject();
  obj["key"] = JsonNull{};

  SECTION("invalid or const char*") {
    std::string result = invalid["key"] or "default";
    REQUIRE(result == "default");
  }

  SECTION("invalid or int") {
    int result = invalid["key"] or 42;
    REQUIRE(result == 42);
  }

  SECTION("invalid or bool") {
    bool result = invalid["key"] or true;
    REQUIRE(result == true);
  }

  SECTION("null or const char*") {
    std::string result = obj["null"] or "default";
    REQUIRE(result == "default");
  }

  SECTION("null or int") {
    int result = obj["null"] or 42;
    REQUIRE(result == 42);
  }

  SECTION("null or bool") {
    bool result = obj["null"] or true;
    REQUIRE(result == true);
  }

  SECTION("int or const char*") {
    obj["key"] = 42;
    std::string result = obj["key"] or "default";
    REQUIRE(result == "default");
  }

  SECTION("int or int") {
    obj["key"] = 0;
    int result = obj["key"] or 666;
    REQUIRE(result == 0);
  }

  SECTION("double or int") {
    obj["key"] = 42.0;
    int result = obj["key"] or 666;
    REQUIRE(result == 42);
  }

  SECTION("bool or bool") {
    obj["key"] = false;
    bool result = obj["key"] or true;
    REQUIRE(result == false);
  }

  SECTION("int or bool") {
    obj["key"] = 0;
    bool result = obj["key"] or true;
    REQUIRE(result == true);
  }

  SECTION("const char* or const char*") {
    obj["key"] = "val";
    std::string result = obj["key"] or "default";
    REQUIRE(result == "val");
  }

  SECTION("const char* or int") {
    obj["key"] = "val";
    int result = obj["key"] or 42;
    REQUIRE(result == 42);
  }
}
