// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#define ARDUINOJSON_ENABLE_COMMENTS 0
#include <ArduinoJson.h>

#include <catch.hpp>
#include <string>

TEST_CASE("Fail parsing comments when comments are disabled") {
  DynamicJsonBuffer jb;

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


