// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonArray subscript ::operator||") {
  JsonArray& invalid = JsonArray::invalid();

  StaticJsonBuffer<JSON_ARRAY_SIZE(1)> jsonBuffer;
  JsonArray& arr = jsonBuffer.createArray();
  arr.add(JsonNull{});

  SECTION("invalid or const char*") {
    std::string result = invalid[0] or "default";
    REQUIRE(result == "default");
  }

  SECTION("invalid or int") {
    int result = invalid[0] or 42;
    REQUIRE(result == 42);
  }

  SECTION("invalid or bool") {
    bool result = invalid[0] or true;
    REQUIRE(result == true);
  }

  SECTION("null or const char*") {
    std::string result = arr[0] or "default";
    REQUIRE(result == "default");
  }

  SECTION("null or int") {
    int result = arr[0] or 42;
    REQUIRE(result == 42);
  }

  SECTION("null or bool") {
    bool result = arr[0] or true;
    REQUIRE(result == true);
  }

  SECTION("int or const char*") {
    arr[0] = 42;
    std::string result = arr[0] or "default";
    REQUIRE(result == "default");
  }

  SECTION("int or int") {
    arr[0] = 0;
    int result = arr[0] or 666;
    REQUIRE(result == 0);
  }

  SECTION("double or int") {
    arr[0] = 42.0;
    int result = arr[0] or 666;
    REQUIRE(result == 42);
  }

  SECTION("bool or bool") {
    arr[0] = false;
    bool result = arr[0] or true;
    REQUIRE(result == false);
  }

  SECTION("int or bool") {
    arr[0] = 0;
    bool result = arr[0] or true;
    REQUIRE(result == true);
  }

  SECTION("const char* or const char*") {
    arr[0] = "val";
    std::string result = arr[0] or "default";
    REQUIRE(result == "val");
  }

  SECTION("const char* or int") {
    arr[0] = "val";
    int result = arr[0] or 42;
    REQUIRE(result == 42);
  }
}
