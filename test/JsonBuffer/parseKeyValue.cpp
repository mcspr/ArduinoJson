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
      [&](JsonVariant, JsonVariant) {
        n++;
      });

    REQUIRE_FALSE(result); // fail due to nesting
    REQUIRE(0 == n); // callback should never execute
    REQUIRE(0 == jsonBuffer.size()); // nothing gets allocated, values inlined in the variant
  }

  SECTION("ObjectNestedInArray") {
    DynamicJsonBuffer jsonBuffer;
    const char jsonString[] =
        " [ { \"a\" : 1 , \"b\" : 2 } , { \"c\" : 3 , \"d\" : 4 } ] ";

    size_t n = 0;
    const auto result = jsonBuffer.parseKeyValue(
      jsonString,
      [&](JsonVariant, JsonVariant) {
        n++;
      });

    REQUIRE_FALSE(result); // is array
    REQUIRE(0 == n); // callback should never execute
    REQUIRE(0 == jsonBuffer.size()); // first element is object, but parser stops before it
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
      [&](JsonVariant key, JsonVariant val) {
        CAPTURE(key.as<const char*>());
        CAPTURE(val.as<const char*>());
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
    const char jsonString[] = "{\"small\": \"val1\", \"and\": \"val2\", \"but not this one that would allocate\": \"last\"}";

    const char* keys[] = {
      "small",
      "and",
    };

    const char* vals[] = {
      "val1",
      "val2",
    };

    size_t n = 0;
    const auto result = jsonBuffer.parseKeyValue(
      jsonString,
      [&](JsonParserStopToken stop, JsonVariant key, JsonVariant val) {
        CAPTURE(key.as<const char*>());
        CAPTURE(val.as<const char*>());
        REQUIRE_THAT(key, Equals(keys[n]));
        REQUIRE_THAT(val, Equals(vals[n]));
        if (n == 1)
          stop();

        n++;
      });

    REQUIRE(result); // parser manually stopped
    REQUIRE(2 == n); // parsing stopped before reaching the end
    REQUIRE(0 == jsonBuffer.size()); // small values never allocate
  }

  SECTION("Stop token & multiple callbacks") {
    StaticJsonBuffer<200> jsonBuffer;
    char tmp1[] = "{hello: world}";

    size_t n = 0;
    auto result = jsonBuffer.parseKeyValue(
      tmp1,
      [&](JsonVariant, JsonVariant) {
        n++;
      });

    REQUIRE(result);
    REQUIRE(1 == n);
    REQUIRE(0 == jsonBuffer.size());

    char tmp2[] = "{key1: val1, key2: val2}";
    result = jsonBuffer.parseKeyValue(
      tmp2,
      [&](JsonParserStopToken stop, JsonVariant, JsonVariant) {
        stop();
        n++;
      });

    REQUIRE(result);
    REQUIRE(2 == n);
    REQUIRE(0 == jsonBuffer.size());

    const char tmp3[] = "{small: obj, small: val}";
    result = jsonBuffer.parseKeyValue(
      tmp3,
      [&](JsonVariant, JsonVariant) {
        n++;
      });

    REQUIRE(result);
    REQUIRE(4 == n);
    REQUIRE(jsonBuffer.size() == 0);

    const char tmp4[] = "{thiskeygetsallocated: obj, small: thisvaluegetsallocated}";
    result = jsonBuffer.parseKeyValue(
      tmp4,
      [&](JsonVariant, JsonVariant) {
        n++;
      });

    REQUIRE(result);
    REQUIRE(6 == n);
    REQUIRE(jsonBuffer.size() > 0);
  }

  SECTION("Empty object") {
    DynamicJsonBuffer jsonBuffer;
    char jsonString[] = "{}";

    size_t n = 0;
    const auto result = jsonBuffer.parseKeyValue(
      jsonString,
      [&](JsonVariant, JsonVariant) {
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
      [&](JsonVariant, JsonVariant) {
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
      [&](JsonVariant, JsonVariant) {
        n++;
      });

    REQUIRE_FALSE(result);
    REQUIRE(0 == n);
    REQUIRE(0 == jsonBuffer.size());
  }
}
