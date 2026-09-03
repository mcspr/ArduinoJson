// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#define ARDUINOJSON_ENABLE_COMMENTS 1
#include <ArduinoJson.h>

#include <catch.hpp>
#include <string>

TEST_CASE("Encountering comments when comments are enabled") {
  DynamicJsonBuffer jb;

  SECTION("Incomplete input w/ or w/o comment") {
    std::string testCases[] = {
      "// comment",
      "// comment\n",
      "/*comment*/",
      "/*\n\ncomment\n\n\t*/",
      "/* comment */",
      "\n/\n",
      "/*\ncomment",
    };

    for (const auto& testCase : testCases) {
      CAPTURE(testCase);
      REQUIRE_FALSE(jb.parse(testCase).success());
    }
  }

  SECTION("Dangling slash") {
    // ref. parseArray / parseObject test cases for dangling slash *after* json end
    std::string failureCases[] = {
      "/{\"hello\":\"world\"}",
      "{/\"hello\":\"world\"}",
      "{\"hello\"/:\"world\"}",
      "{\"hello\":/\"world\"}",
      "{\"hello\":\"world\"/}",
      "{\"hello\":\"world\"/,\"answer\":42}",
      "{\"hello\":\"world\",/\"answer\":42}",
      "{\"hello\":\"world\",\"answer\"/:42}",
      "{\"hello\":\"world\",\"answer\":/42}",
      "{\"hello\":\"world\",\"answer\":42/}",
      "/[\"hello\",\"world\"]",
      "[/\"hello\",\"world\"]",
      "[\"hello\"/,\"world\"]",
      "[\"hello\",/\"world\"]",
      "[\"hello\",\"world\"/]",
    };

    for (const auto& testCase : failureCases) {
      CAPTURE(testCase);
      REQUIRE_FALSE(jb.parse(testCase).success());
    }
  }

  SECTION("Single-line comments") {
    // ref. https://jsonc.org/#single-line-comments
    std::string testCase = R"(
{
    // This is a single-line comment
    "name": "John Doe",
    "age": 30 // This is another single-line comment
}
    )";

    JsonVariant variant = jb.parse(testCase);
    REQUIRE(variant.success());
    REQUIRE(variant.is<JsonObject>());

    JsonObject& obj = variant.as<JsonObject&>();
    REQUIRE(2 == obj.size());
    REQUIRE(obj["name"] == "John Doe");
    REQUIRE(obj["age"] == 30);
  }

  SECTION("Multi-line comments") {
    // ref. https://jsonc.org/#multi-line-comments
    std::string testCase = R"(
{
    /*
      This is a block comment
      that spans multiple lines
    */
    "name": "Jane Doe",
    "age": /* This is a single-line block comment */ 25
})";

    JsonVariant variant = jb.parse(testCase);
    REQUIRE(variant.success());
    REQUIRE(variant.is<JsonObject>());

    JsonObject& obj = variant.as<JsonObject&>();
    REQUIRE(2 == obj.size());
    REQUIRE(obj["name"] == "Jane Doe");
    REQUIRE(obj["age"] == 25);
  }

  SECTION("Nested multi-line comments") {
    // ref. https://jsonc.org/#multi-line-comments
    std::string testCase = R"(
{
/* OUTER start
  /* NESTED block comments are not supported.
      OUTER block comment will end here --> */

  OUTER end
*/
    "name": "John Doe",
    "age": 30
}
    )";

    REQUIRE_FALSE(jb.parse(testCase).success());
  }
}


