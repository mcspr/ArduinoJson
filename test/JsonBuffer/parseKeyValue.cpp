// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

using Catch::Equals;

TEST_CASE("JsonBuffer kv objects") {
  SECTION("ArrayNestedInObject") {
    DynamicJsonBuffer jsonBuffer;
    const char jsonString[] = " { \"ab\" : [ 1 , 2 ] , \"cd\" : [ 3 , 4 ] } ";

    size_t n = 0;
    const auto result = jsonBuffer.parseKeyValue(
      jsonString,
      [&](const char*, JsonVariant) {
        n++;
      });

    REQUIRE_FALSE(result); // fail due to nesting
    REQUIRE(0 == n); // callback should never execute
    REQUIRE(3 == jsonBuffer.size()); // "ab"
  }

  SECTION("ObjectNestedInArray") {
    DynamicJsonBuffer jsonBuffer;
    const char jsonString[] =
        " [ { \"a\" : 1 , \"b\" : 2 } , { \"c\" : 3 , \"d\" : 4 } ] ";

    size_t n = 0;
    const auto result = jsonBuffer.parseKeyValue(
      jsonString,
      [&](const char*, JsonVariant) {
        n++;
      });

    REQUIRE_FALSE(result); // is array
    REQUIRE(0 == n); // callback should never execute
    REQUIRE(0 == jsonBuffer.size()); // first element is object, which should not be allocated
  }

  SECTION("Simple key value") {
    DynamicJsonBuffer jsonBuffer;
    const char jsonString[] = "{a: first, b: second}";

    const char* keys[] = {
      "a",
      "b",
    };

    const char* vals[] = {
      "first",
      "second",
    };

    size_t n = 0;
    const auto result = jsonBuffer.parseKeyValue(
      jsonString,
      [&](const char* key, JsonVariant val) {
        CAPTURE(key);
        CAPTURE(val);
        REQUIRE_THAT(key, Equals(keys[n]));
        REQUIRE_THAT(val, Equals(vals[n]));
        ++n;
        jsonBuffer.clear();
      });

    REQUIRE(result);
    REQUIRE(2 == n); // executed exactly once per key
    REQUIRE(0 == jsonBuffer.size()); // all allocations are cleared
  }

  SECTION("Stop token") {
    DynamicJsonBuffer jsonBuffer;
    const char jsonString[] = "{\"process\": \"first\", \"and\": \"second\", \"but not\": \"last\"}";

    const char* keys[] = {
      "process",
      "and",
    };

    const char* vals[] = {
      "first",
      "second",
    };

    size_t n = 0;
    const auto result = jsonBuffer.parseKeyValue(
      jsonString,
      [&](JsonParserStopToken stop, const char* key, JsonVariant val) {
        CAPTURE(key);
        CAPTURE(val);
        REQUIRE_THAT(key, Equals(keys[n]));
        REQUIRE_THAT(val, Equals(vals[n]));
        if (n == 1)
          stop();

        n++;
      });

    REQUIRE(result); // parser manually stopped
    REQUIRE(2 == n); // parsing stopped before reaching the end
    REQUIRE(jsonBuffer.size() ==
      (std::strlen(keys[0]) +
       std::strlen(keys[1]) +
       std::strlen(vals[0]) +
       std::strlen(vals[1]) + 4));
  }

  SECTION("Empty object") {
    DynamicJsonBuffer jsonBuffer;
    char jsonString[] = "{}";

    size_t n = 0;
    const auto result = jsonBuffer.parseKeyValue(
      jsonString,
      [&](const char*, JsonVariant) {
        n++;
      });

    REQUIRE(result);
    REQUIRE(0 == n);
    REQUIRE(0 == jsonBuffer.size());
  }

  SECTION("Empty array") {
    DynamicJsonBuffer jsonBuffer;
    char jsonString[] = "[]";

    size_t n = 0;
    const auto result = jsonBuffer.parseKeyValue(
      jsonString,
      [&](const char*, JsonVariant) {
        n++;
      });

    REQUIRE_FALSE(result);
    REQUIRE(0 == n);
    REQUIRE(0 == jsonBuffer.size());
  }

  SECTION("Random string") {
    DynamicJsonBuffer jsonBuffer;
    char jsonString[] = "\"hello world\"";

    size_t n = 0;
    const auto result = jsonBuffer.parseKeyValue(
      jsonString,
      [&](const char*, JsonVariant) {
        n++;
      });

    REQUIRE_FALSE(result);
    REQUIRE(0 == n);
    REQUIRE(0 == jsonBuffer.size());
  }
}
