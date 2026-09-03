// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#define ARDUINOJSON_ENABLE_COMMENTS 0
#include <ArduinoJson.h>

#include <catch.hpp>
#include <string>

TEST_CASE("Fail parsing comments when comments are disabled") {
  DynamicJsonBuffer jb;

  SECTION("Various comments") {
    std::string testCases[] = {
      "/*COMMENT*/  [\"hello\"]",
      "[/*COMMENT*/ \"hello\"]",
      "[\"hello\"/*COMMENT*/]",
      "[\"hello\"/*COMMENT*/,\"world\"]",
      "[\"hello\",/*COMMENT*/ \"world\"]",
      "[/*/\n]",
      "[/*COMMENT]",
      "[/*COMMENT*]",
      "//COMMENT\n\t[\"hello\"]",
      "[//COMMENT\n\"hello\"]",
      "[\"hello\"//COMMENT\r\n]",
      "[\"hello\"//COMMENT\n,\"world\"]",
      "[\"hello\",//COMMENT\n\"world\"]",
      "[/COMMENT\n]",
      "[//COMMENT",
      "/*COMMENT*/ {\"hello\":\"world\"}",
      "{/*COMMENT*/\"hello\":\"world\"}",
      "{\"hello\"/*COMMENT*/:\"world\"}",
      "{\"hello\":/*COMMENT*/\"world\"}",
      "{\"hello\":\"world\"/*COMMENT*/}",
      "//COMMENT\n {\"hello\":\"world\"}",
      "{//COMMENT\n\"hello\":\"world\"}",
      "{\"hello\"//COMMENT\n:\"world\"}",
      "{\"hello\"://COMMENT\n\"world\"}",
      "{\"hello\":\"world\"//COMMENT\n}",
      "/{\"hello\":\"world\"}",
      "{/\"hello\":\"world\"}",
      "{\"hello\"/:\"world\"}",
      "{\"hello\":/\"world\"}",
      "{\"hello\":\"world\"/}",
      "{\"hello\":\"world\"/,\"answer\":42}",
      "{\"hello\":\"world\",/\"answer\":42}",
    };

    for (const auto& testCase  : testCases) {
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

    REQUIRE_FALSE(jb.parse(testCase).success());
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

    REQUIRE_FALSE(jb.parse(testCase).success());
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


