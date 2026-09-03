// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

static std::string makeNestedObject(size_t nesting) {
  static const std::string head = "{key:";

  std::string str;
  str.reserve(((nesting - 1) * head.length()) + nesting);

  for (size_t n = 0; n < nesting - 1; ++n) {
    str += head;
  }
  str += '{';
  str.append(nesting, '}');

  return str;
}

static std::string makeNestedArray(size_t nesting) {
  std::string str;
  str.reserve(nesting * 2);

  str.append(nesting, '[');
  str.append(nesting, ']');

  return str;
}

TEST_CASE("JsonParser nestingLimit") {
  DynamicJsonBuffer jb(JSON_OBJECT_SIZE(256) + JSON_ARRAY_SIZE(256));

  SECTION("parseArray()") {
    SECTION("limit = 0") {
      REQUIRE_FALSE(jb.parseArray("[]", {0}).success());
    }

    SECTION("limit = 1") {
      REQUIRE(jb.parseArray("[]", {1}).success());
      REQUIRE_FALSE(jb.parseArray("[[]]", {1}).success());
    }

    SECTION("limit = 2") {
      REQUIRE(jb.parseArray("[[]]", {2}).success());
      REQUIRE(jb.parseArray("[[1,2,3]]", {2}).success());
      REQUIRE(jb.parseArray("[{\"a\":123},{\"key\":\"val\"}]", {2}).success());
      REQUIRE(jb.parseArray("[[],[12,34]]", {2}).success());
      REQUIRE_FALSE(jb.parseArray("[1, [[]]]", {2}).success());
      REQUIRE_FALSE(jb.parseArray("[[[]], 2]", {2}).success());
    }

    SECTION("limit = 254") {
      REQUIRE(jb.parseArray(makeNestedArray(254), {254}).success());
      REQUIRE_FALSE(jb.parseArray(makeNestedArray(255), {254}).success());
    }

    SECTION("limit = 255") {
      REQUIRE(jb.parseArray(makeNestedArray(254), {255}).success());
      REQUIRE_FALSE(jb.parseArray(makeNestedArray(255), {255}).success());
    }
  }

  SECTION("parseObject()") {
    SECTION("limit = 0") {
      REQUIRE_FALSE(jb.parseObject("{}", {0}).success());
    }

    SECTION("limit = 1") {
      REQUIRE(jb.parseObject("{\"key1\":42,\"key2\":\"val\"}", {1}).success());
      REQUIRE_FALSE(jb.parseObject("{\"key\":{\"key\":42}}", {1}).success());
    }

    SECTION("limit = 2") {
      REQUIRE(jb.parseObject("{\"key\":{\"nested1\":42,\"nested2\":null}}", {2}).success());
      REQUIRE_FALSE(jb.parseObject("{\"key\":{\"key\":{\"key\":42}}}", {2}).success());
    }

    SECTION("limit = 254") {
      REQUIRE(jb.parseObject(makeNestedObject(254), {254}).success());
      REQUIRE_FALSE(jb.parseObject(makeNestedObject(255), {254}).success());
    }

    SECTION("limit = 255") {
      REQUIRE(jb.parseObject(makeNestedObject(254), {255}).success());
      REQUIRE_FALSE(jb.parseObject(makeNestedObject(255), {255}).success());
    }
  }

  SECTION("parse()") {
    SECTION("limit = 0") {
      REQUIRE(jb.parse("\"toto\"", {0}).success());  // JsonVariant::success()
      REQUIRE(jb.parse("null", {0}).success());
      REQUIRE(jb.parse("1.2345", {0}).success());
      REQUIRE(jb.parse("123", {0}).success());
      REQUIRE(jb.parse("false", {0}).success());
      REQUIRE(jb.parse("true", {0}).success());
      REQUIRE_FALSE(jb.parse("[]", {0}).success());
      REQUIRE_FALSE(jb.parse("[[]]", {0}).success());
      REQUIRE_FALSE(jb.parse("{}", {0}).success());
      REQUIRE_FALSE(jb.parse("{\"toto\":{}}", {0}).success());
      REQUIRE_FALSE(jb.parse("[\"toto\"]", {0}).success());
      REQUIRE_FALSE(jb.parse("{\"toto\":1}", {0}).success());
    }

    SECTION("limit = 1") {
      REQUIRE(jb.parse("[\"toto\"]", {1}).success());
      REQUIRE(jb.parse("{\"toto\":null}", {1}).success());
      REQUIRE(jb.parse("{\"toto\":24.5}", {1}).success());
      REQUIRE(jb.parse("{\"toto\":123}", {1}).success());
      REQUIRE(jb.parse("{\"toto\":false}", {1}).success());
      REQUIRE(jb.parse("{\"toto\":true}", {1}).success());
      REQUIRE(jb.parse("{\"toto\":\"toto\"}", {1}).success());
      REQUIRE_FALSE(jb.parse("{\"toto\":{}}", {1}).success());
      REQUIRE_FALSE(jb.parse("{\"toto\":[]}", {1}).success());
      REQUIRE_FALSE(jb.parse("{\"toto\":[1,2,3]}", {1}).success());
      REQUIRE_FALSE(jb.parse("{\"toto\":[,]}", {1}).success());
      REQUIRE_FALSE(jb.parse("[[\"toto\"]]", {1}).success());
      REQUIRE_FALSE(jb.parse("[{\"toto\":1}]", {1}).success());
    }
  }
}
