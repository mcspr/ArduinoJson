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
}


