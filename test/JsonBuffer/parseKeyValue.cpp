// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

using Catch::Equals;

TEST_CASE("JsonBuffer::parseKeyValue()") {
  SECTION("DynamicJsonBuffer") {
    DynamicJsonBuffer jb;

    SECTION("Array values") {
      static constexpr const char jsonString[] = R"(
{
  "ab" : [ 1 , 2 ],
  "cd" : [ 3 , 4 ]
})";

      size_t n = 0;
      const auto result = jb.parseKeyValue(
        jsonString,
        [&](JsonVariant, JsonVariant) {
          n++;
        });

      REQUIRE_FALSE(result); // fail due to nesting
      REQUIRE(0 == n); // callback should never execute
      REQUIRE(0 == jb.size()); // nothing gets allocated, values inlined in the variant
    }

    SECTION("Array instead of object") {
      static constexpr char jsonString[] = R"(
[
  { "a" : 1 ,
    "b" : 2
  },
  {
    "c" : 3 ,
    "d" : 4
  }
])";

      size_t n = 0;
      const auto result = jb.parseKeyValue(
        jsonString,
        [&](JsonVariant, JsonVariant) {
          n++;
        });

      REQUIRE_FALSE(result); // is array
      REQUIRE(0 == n); // callback should never execute
      REQUIRE(0 == jb.size()); // first element is object, but parser stops before it
    }

    SECTION("Simple key value") {
      static constexpr char jsonString[] = R"(
  {
    a: first,
    b: second
  })";

      static constexpr const char* keys[] = {
        "a",
        "b",
      };

      const char* vals[] = {
        "first",
        "second",
      };

      size_t n = 0;
      const auto result = jb.parseKeyValue(
        jsonString,
        [&](JsonVariant key, JsonVariant val) {
          CAPTURE(key.as<const char*>());
          CAPTURE(val.as<const char*>());
          REQUIRE_THAT(key, Equals(keys[n]));
          REQUIRE_THAT(val, Equals(vals[n]));
          ++n;
          jb.clear();
        });

      REQUIRE(result);
      REQUIRE(2 == n); // executed exactly once per key
      REQUIRE(0 == jb.size()); // all allocations are cleared
    }

    SECTION("Stop token") {
      static constexpr char jsonString[] = R"(
  {
    "small": "val1",
    "and": "val2",
    "constant char pointer is used to": "ensure json buffer is selected to be written to",
    "these key and value would be parsed": "because of the stop token signal"
  })";

      static constexpr const char* keys[] = {
        "small",
        "and",
      };

      static constexpr const char* vals[] = {
        "val1",
        "val2",
      };

      size_t n = 0;
      const auto result = jb.parseKeyValue(
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
      REQUIRE(0 == jb.size()); // small values never allocate
    }

    SECTION("Empty object") {
      char jsonString[] = "{}";

      size_t n = 0;
      const auto result = jb.parseKeyValue(
        jsonString,
        [&](JsonVariant, JsonVariant) {
          n++;
        });

      REQUIRE(result);
      REQUIRE(0 == n);
      REQUIRE(0 == jb.size());
    }

    SECTION("Empty array") {
      char jsonString[] = "[]";

      size_t n = 0;
      const auto result = jb.parseKeyValue(
        jsonString,
        [&](JsonVariant, JsonVariant) {
          n++;
        });

      REQUIRE_FALSE(result);
      REQUIRE(0 == n);
      REQUIRE(0 == jb.size());
    }

    SECTION("Double quoted string") {
      char jsonString[] = "\"hello world\"";

      size_t n = 0;
      const auto result = jb.parseKeyValue(
        jsonString,
        [&](JsonVariant, JsonVariant) {
          n++;
        });

      REQUIRE_FALSE(result);
      REQUIRE(0 == n);
      REQUIRE(0 == jb.size());
    }

    SECTION("Single quoted string") {
      char jsonString[] = "'hello world'";

      size_t n = 0;
      const auto result = jb.parseKeyValue(
        jsonString,
        [&](JsonVariant, JsonVariant) {
          n++;
        });

      REQUIRE_FALSE(result);
      REQUIRE(0 == n);
      REQUIRE(0 == jb.size());
    }

    SECTION("Unquoted string") {
      char jsonString[] = "helloworld";

      size_t n = 0;
      const auto result = jb.parseKeyValue(
        jsonString,
        [&](JsonVariant, JsonVariant) {
          n++;
        });

      REQUIRE_FALSE(result);
      REQUIRE(0 == n);
      REQUIRE(0 == jb.size());
    }
  }

  SECTION("StaticJsonBuffer") {
    StaticJsonBuffer<256> jb;

    SECTION("Stop token & multiple callbacks") {
      char tmp1[] = "{hello: world}";

      size_t n = 0;
      auto result = jb.parseKeyValue(
        tmp1,
        [&](JsonVariant, JsonVariant) {
          n++;
        });

      REQUIRE(result);
      REQUIRE(1 == n);
      REQUIRE(0 == jb.size());

      char tmp2[] = "{key1: val1, key2: val2}";
      result = jb.parseKeyValue(
        tmp2,
        [&](JsonParserStopToken stop, JsonVariant, JsonVariant) {
          stop();
          n++;
        });

      REQUIRE(result);
      REQUIRE(2 == n);
      REQUIRE(0 == jb.size());

      const char tmp3[] = "{small: obj, small: val}";
      result = jb.parseKeyValue(
        tmp3,
        [&](JsonVariant, JsonVariant) {
          n++;
        });

      REQUIRE(result);
      REQUIRE(4 == n);
      REQUIRE(jb.size() == 0);

      const char tmp4[] = "{thiskeygetsallocated: obj, small: thisvaluegetsallocated}";
      result = jb.parseKeyValue(
        tmp4,
        [&](JsonVariant, JsonVariant) {
          n++;
        });

      REQUIRE(result);
      REQUIRE(6 == n);
      REQUIRE(jb.size() > 0);
    }
  }
}
