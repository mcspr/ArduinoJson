// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#define ARDUINOJSON_ENABLE_COMMENTS 1
#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonBuffer::parseArray() w/ Comments") {
  DynamicJsonBuffer jb;

  SECTION("CCommentBeforeOpeningBracket") {
    JsonArray& arr = jb.parseArray("/*COMMENT*/  [\"hello\"]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("CCommentAfterOpeningBracket") {
    JsonArray& arr = jb.parseArray("[/*COMMENT*/ \"hello\"]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("CCommentBeforeClosingBracket") {
    JsonArray& arr = jb.parseArray("[\"hello\"/*COMMENT*/]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("CCommentAfterClosingBracket") {
    JsonArray& arr = jb.parseArray("[\"hello\"]/*COMMENT*/");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("CCommentBeforeComma") {
    JsonArray& arr = jb.parseArray("[\"hello\"/*COMMENT*/,\"world\"]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == "hello");
    REQUIRE(arr[1] == "world");
  }

  SECTION("CCommentAfterComma") {
    JsonArray& arr = jb.parseArray("[\"hello\",/*COMMENT*/ \"world\"]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == "hello");
    REQUIRE(arr[1] == "world");
  }

  SECTION("CppCommentBeforeOpeningBracket") {
    JsonArray& arr = jb.parseArray("//COMMENT\n\t[\"hello\"]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("CppCommentAfterOpeningBracket") {
    JsonArray& arr = jb.parseArray("[//COMMENT\n\"hello\"]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("CppCommentBeforeClosingBracket") {
    JsonArray& arr = jb.parseArray("[\"hello\"//COMMENT\r\n]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("CppCommentAfterClosingBracket") {
    JsonArray& arr = jb.parseArray("[\"hello\"]//COMMENT\n");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("CppCommentBeforeComma") {
    JsonArray& arr = jb.parseArray("[\"hello\"//COMMENT\n,\"world\"]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == "hello");
    REQUIRE(arr[1] == "world");
  }

  SECTION("CppCommentAfterComma") {
    JsonArray& arr = jb.parseArray("[\"hello\",//COMMENT\n\"world\"]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == "hello");
    REQUIRE(arr[1] == "world");
  }

  SECTION("InvalidCppComment") {
    JsonArray& arr = jb.parseArray("[/COMMENT\n]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("InvalidComment") {
    JsonArray& arr = jb.parseArray("[/*/\n]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("UnfinishedCComment") {
    JsonArray& arr = jb.parseArray("[/*COMMENT]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("EndsInCppComment") {
    JsonArray& arr = jb.parseArray("[//COMMENT");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("AfterClosingStar") {
    JsonArray& arr = jb.parseArray("[/*COMMENT*");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("DeeplyNestedSuccess") {
    const char input[] = "[[[[/* some */[[[[/***comments\n*/[[[[[[[[[[[\"Not too deep\"]]/*within*/]]]]]]]]]]]]]]]]]// trailing";
    REQUIRE(jb.parseArray(input, 19).success());
  }

  SECTION("DeeplyNestedFailure") {
    const char input[] = "[[[[[[[[[[[[[[[[[[/* whatever */[\"Not too deep\"]]]]]]]]]]]]]]]]]]]";
    REQUIRE_FALSE(jb.parseArray(input, 18).success());
  }
}
