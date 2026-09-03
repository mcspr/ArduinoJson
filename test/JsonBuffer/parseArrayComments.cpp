// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#define ARDUINOJSON_ENABLE_COMMENTS 1
#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonBuffer::parseArray() w/ Comments") {
  DynamicJsonBuffer jb;

  SECTION("Multi-line comment before opening bracket") {
    JsonArray& arr = jb.parseArray("/*COMMENT*/  [\"hello\"]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("Multi-line comment after opening bracket") {
    JsonArray& arr = jb.parseArray("[/*COMMENT*/ \"hello\"]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("Multi-line comment before closing bracket") {
    JsonArray& arr = jb.parseArray("[\"hello\"/*COMMENT*/]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("Multi-line comment after closing bracket") {  // parser stopped before encountering the comment
    JsonArray& arr = jb.parseArray("[\"hello\"]/*COMMENT*/");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("Multi-line comment before comma") {
    JsonArray& arr = jb.parseArray("[\"hello\"/*COMMENT*/,\"world\"]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == "hello");
    REQUIRE(arr[1] == "world");
  }

  SECTION("Multi-line comment after comma") {
    JsonArray& arr = jb.parseArray("[\"hello\",/*COMMENT*/ \"world\"]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == "hello");
    REQUIRE(arr[1] == "world");
  }

  SECTION("Single-line comment before opening bracket") {
    JsonArray& arr = jb.parseArray("//COMMENT\n\t[\"hello\"]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("Single-line comment after opening bracket") {
    JsonArray& arr = jb.parseArray("[//COMMENT\n\"hello\"]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("Single-line comment before closing bracket") {
    JsonArray& arr = jb.parseArray("[\"hello\"//COMMENT\r\n]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("Single-line comment after closing bracket") {  // parser stopped before encountering the comment
    JsonArray& arr = jb.parseArray("[\"hello\"]//COMMENT\n");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("Single-line comment before comma") {
    JsonArray& arr = jb.parseArray("[\"hello\"//COMMENT\n,\"world\"]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == "hello");
    REQUIRE(arr[1] == "world");
  }

  SECTION("Single-line comment after comma") {
    JsonArray& arr = jb.parseArray("[\"hello\",//COMMENT\n\"world\"]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == "hello");
    REQUIRE(arr[1] == "world");
  }

  SECTION("Invalid single-line comment") {
    JsonArray& arr = jb.parseArray("[/COMMENT\n]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Invalid multi-line comment") {
    JsonArray& arr = jb.parseArray("[/*/\n]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Unfinished multi-line comment") {
    JsonArray& arr = jb.parseArray("[/*COMMENT]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Ends with single-line comment") {
    JsonArray& arr = jb.parseArray("[//COMMENT");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Unfinished array ends with unfinished multi-line comment") {
    JsonArray& arr = jb.parseArray("[/*COMMENT*");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Deeply nested success") {
    const char input[] = "[// allowing\n[[[/* some */[[[[/***comments\n*/[[[[[[[[[[[\"Not too deep\"]]/*within*/]]]]]]]]]// this array\r\n]]]]]]]]// trailing";
    REQUIRE(jb.parseArray(input, 19).success());
  }

  SECTION("Deeply nested failure") {
    const char input[] = "[[[[[[[[[[[[[[[[[[/* whatever */[\"Not too deep\"]]]]]]]]]]]]]]]]]]]";
    REQUIRE_FALSE(jb.parseArray(input, 18).success());
  }
}
