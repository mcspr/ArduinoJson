// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

static void check(JsonArray &array, std::string expected) {
  std::string actual;
  size_t actualLen = array.printTo(actual);
  REQUIRE(expected == actual);
  REQUIRE(actualLen == expected.size());
  size_t measuredLen = array.measureLength();
  REQUIRE(measuredLen == expected.size());
}

TEST_CASE("JsonArray::printTo()") {
  StaticJsonBuffer<JSON_ARRAY_SIZE(2)> jb;
  JsonArray &array = jb.createArray();

  SECTION("Empty") {
    check(array, "[]");
  }

  SECTION("NullPtr") {
    REQUIRE_FALSE(array.add(static_cast<const char *>(nullptr)));

    check(array, "[]");
  }

  SECTION("Null") {
    REQUIRE(array.add(JsonNull{}));

    check(array, "[null]");
  }

  SECTION("OneString") {
    REQUIRE(array.add("hello"));

    check(array, "[\"hello\"]");
  }

  SECTION("TwoStrings") {
    REQUIRE(array.add("hello"));
    REQUIRE(array.add("world"));

    check(array, "[\"hello\",\"world\"]");
  }

  SECTION("OneStringOverCapacity") {
    REQUIRE(array.add("hello"));
    REQUIRE(array.add("world"));
    REQUIRE_FALSE(array.add("lost"));

    check(array, "[\"hello\",\"world\"]");
  }

  SECTION("One double") {
    REQUIRE(array.add(3.1415927));

    check(array, "[3.1415927]");
  }

  SECTION("OneInteger") {
    REQUIRE(array.add(1));

    check(array, "[1]");
  }

  SECTION("TwoIntegers") {
    REQUIRE(array.add(1));
    REQUIRE(array.add(2));

    check(array, "[1,2]");
  }

  SECTION("RawJson(const char*)") {
    REQUIRE(array.add(RawJson("{\"key\":\"value\"}")));

    check(array, "[{\"key\":\"value\"}]");
  }

  SECTION("RawJson(char*)") {
    DynamicJsonBuffer jb2;
    JsonArray &arr = jb2.createArray();

    char tmp[] = "{\"key\":\"value\"}";
    REQUIRE(arr.add(RawJson(tmp)));

    check(arr, "[{\"key\":\"value\"}]");
  }

  SECTION("OneIntegerOverCapacity") {
    REQUIRE(array.add(1));
    REQUIRE(array.add(2));
    REQUIRE_FALSE(array.add(3));

    check(array, "[1,2]");
  }

  SECTION("OneTrue") {
    REQUIRE(array.add(true));

    check(array, "[true]");
  }

  SECTION("OneFalse") {
    REQUIRE(array.add(false));

    check(array, "[false]");
  }

  SECTION("TwoBooleans") {
    REQUIRE(array.add(false));
    REQUIRE(array.add(true));

    check(array, "[false,true]");
  }

  SECTION("OneBooleanOverCapacity") {
    REQUIRE(array.add(false));
    REQUIRE(array.add(true));
    REQUIRE_FALSE(array.add(false));

    check(array, "[false,true]");
  }

  SECTION("OneEmptyNestedArray") {
    const auto& arr = array.createNestedArray();
    REQUIRE(arr.success());

    check(array, "[[]]");
  }

  SECTION("OneEmptyNestedHash") {
    const auto& obj = array.createNestedObject();
    REQUIRE(obj.success());

    check(array, "[{}]");
  }
}
