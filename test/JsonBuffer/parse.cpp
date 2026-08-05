// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

using Catch::Matchers::Equals;

TEST_CASE("JsonBuffer::parse()") {
  DynamicJsonBuffer jb;

  SECTION("EmptyObject") {
    JsonVariant variant = jb.parse("{}");
    REQUIRE(variant.success());
    REQUIRE(variant.is<JsonObject>());
  }

  SECTION("EmptyArray") {
    JsonVariant variant = jb.parse("[]");
    REQUIRE(variant.success());
    REQUIRE(variant.is<JsonArray>());
  }

  SECTION("Integer") {
    JsonVariant variant = jb.parse("-42");
    REQUIRE(variant.success());
    REQUIRE(variant.is<int>());
    REQUIRE_FALSE(variant.is<bool>());
    REQUIRE(variant == -42);
  }

  SECTION("Double") {
    JsonVariant variant = jb.parse("-1.23e+4");
    REQUIRE(variant.success());
    REQUIRE_FALSE(variant.is<int>());
    REQUIRE(variant.is<double>());
    REQUIRE(variant.as<double>() == Approx(-1.23e+4));
  }

  SECTION("Double quoted string") {
    JsonVariant variant = jb.parse("\"hello world\"");
    REQUIRE(variant.success());
    REQUIRE(variant.is<char*>());
    REQUIRE_THAT(variant.as<char*>(), Equals("hello world"));
  }

  SECTION("Single quoted string") {
    JsonVariant variant = jb.parse("\'hello world\'");
    REQUIRE(variant.success());
    REQUIRE(variant.is<char*>());
    REQUIRE_THAT(variant.as<char*>(), Equals("hello world"));
  }

  SECTION("Truncated JSON string") {
    std::string testCases[] = {"\"hello", "\'hello", "'\\u", "'\\u00", "'\\u000"};

    for (const auto& testCase : testCases) {
      CAPTURE(testCase);

      JsonVariant variant = jb.parse(testCase);
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<char*>());
    }
  }

  SECTION("Escape single quote in single quoted string") {
    JsonVariant variant = jb.parse("'ab\\\'cd'");
    REQUIRE(variant.success());
    REQUIRE(variant.is<char*>());
    REQUIRE(variant.as<std::string>() == "ab\'cd");
  }

  SECTION("Invalid JSON string") {
    std::string testCases[] = {"'\\u'",     "'\\u000g'", "'\\u000'",
                               "'\\u000G'", "'\\u000/'", "'\\x1234'"};

    for (const auto& testCase : testCases) {
      CAPTURE(testCase);

      JsonVariant variant = jb.parse(testCase);
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<char*>());
    }
  }

  SECTION("True") {
    JsonVariant variant = jb.parse("true");
    REQUIRE(variant.success());
    REQUIRE(variant.is<bool>());
    REQUIRE(variant.as<bool>() == true);
    REQUIRE(variant == true);
  }

  SECTION("False") {
    JsonVariant variant = jb.parse("false");
    REQUIRE(variant.success());
    REQUIRE(variant.is<bool>());
    REQUIRE(variant.as<bool>() == false);
    REQUIRE(variant == false);
  }

  SECTION("OpenBrace") {
    JsonVariant variant = jb.parse("{");
    REQUIRE_FALSE(variant.success());
  }

  SECTION("Incomplete string") {
    JsonVariant variant = jb.parse("\"hello");
    REQUIRE_FALSE(variant.success());
  }
}
