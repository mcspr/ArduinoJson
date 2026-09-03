// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonBuffer::parseArray()") {
  DynamicJsonBuffer jb;

  SECTION("Empty array") {
    JsonArray& arr = jb.parseArray("[]");

    REQUIRE(arr.success());
    REQUIRE(0 == arr.size());
  }

  SECTION("Missing opening bracket") {
    JsonArray& arr = jb.parseArray("]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Array with no end") {
    JsonArray& arr = jb.parseArray("[");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Empty array with leading spaces") {
    JsonArray& arr = jb.parseArray("  []");

    REQUIRE(arr.success());
    REQUIRE(0 == arr.size());
  }

  SECTION("non-printable ASCII garbage") {
    JsonArray& arr = jb.parseArray("%*$£¤");

    REQUIRE_FALSE(arr.success());
  }

  SECTION("One integer") {
    JsonArray& arr = jb.parseArray("[42]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == 42);
  }

  SECTION("One integer with spaces before") {
    JsonArray& arr = jb.parseArray("[ \t\r\n42]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == 42);
  }

  SECTION("One integer with space after") {
    JsonArray& arr = jb.parseArray("[42 \t\r\n]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == 42);
  }

  SECTION("One integer with comma after") {
    JsonArray& arr = jb.parseArray("[42,]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("One integer with comma before") {
    JsonArray& arr = jb.parseArray("[,42]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Two integers") {
    JsonArray& arr = jb.parseArray("[42,84]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == 42);
    REQUIRE(arr[1] == 84);
  }

  SECTION("Two doubles") {
    JsonArray& arr = jb.parseArray("[4.2,1e2]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == Approx(4.2));
    REQUIRE(arr[1] == 1e2);
  }

  SECTION("Unsigned long") {
    JsonArray& arr = jb.parseArray("[4294967295]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == 4294967295UL);
  }

  SECTION("Two booleans") {
    JsonArray& arr = jb.parseArray("[true,false]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == true);
    REQUIRE(arr[1] == false);
  }

  SECTION("Two nulls") {
    JsonArray& arr = jb.parseArray("[null,null]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0].is<JsonNull>());
    REQUIRE(arr[1].is<JsonNull>());
    REQUIRE(arr[0].as<char*>() == std::string("null"));
    REQUIRE(arr[1].as<char*>() == std::string("null"));
  }

  SECTION("Two strings double quotes") {
    JsonArray& arr = jb.parseArray("[ \"hello\" , \"world\" ]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == "hello");
    REQUIRE(arr[1] == "world");
  }

  SECTION("Two strings single quotes") {
    JsonArray& arr = jb.parseArray("[ 'hello' , 'world' ]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == "hello");
    REQUIRE(arr[1] == "world");
  }

  SECTION("Two strings no quotes") {
    JsonArray& arr = jb.parseArray("[ hello , world ]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == "hello");
    REQUIRE(arr[1] == "world");
  }

  SECTION("Empty strings double quotes") {
    JsonArray& arr = jb.parseArray("[\"\",\"\"]");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == "");
    REQUIRE(arr[1] == "");
  }

  SECTION("Empty string single quotes") {
    JsonArray& arr = jb.parseArray("[\'\',\'\']");

    REQUIRE(arr.success());
    REQUIRE(2 == arr.size());
    REQUIRE(arr[0] == "");
    REQUIRE(arr[1] == "");
  }

  SECTION("Empty string no quotes") {
    JsonArray& arr = jb.parseArray("[,]");

    REQUIRE_FALSE(arr.success());
  }

  SECTION("Closing double quote missing") {
    JsonArray& arr = jb.parseArray("[\"]");

    REQUIRE_FALSE(arr.success());
  }

  SECTION("Closing signle quote missing") {
    JsonArray& arr = jb.parseArray("[\']");

    REQUIRE_FALSE(arr.success());
  }

  SECTION("String with escaped chars") {
    JsonArray& arr = jb.parseArray("[\"1\\\"2\\\\3\\/4\\b5\\f6\\n7\\r8\\t9\"]");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "1\"2\\3/4\b5\f6\n7\r8\t9");
  }

  SECTION("String with null byte") {
    JsonArray& arr = jb.parseArray("['\0','\1', '\2']", 1);
    REQUIRE_FALSE(arr.success());
  }

  SECTION("String with control character") {
    JsonArray& arr = jb.parseArray("['\1', '\2', '\3']", 1);
    REQUIRE_FALSE(arr.success());
  }

  SECTION("String with unterminated escape Sequence") {
    JsonArray& arr = jb.parseArray("\"\\\0\"", 4);
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Deeply nested failure") {
    const char input[] = "[[[[[[[[[[[[[[[[[[[\"Not too deep\"]]]]]]]]]]]]]]]]]]]";
    REQUIRE_FALSE(jb.parseArray(input, 18).success());
  }

  SECTION("Deeply nested success") {
    const char input[] = "[[[[[[[[[[[[[[[[[[[\"Not too deep\"]]]]]]]]]]]]]]]]]]]";
    REQUIRE(jb.parseArray(input, 19).success());
  }

  SECTION("Multi-line comment before opening bracket") {
    JsonArray& arr = jb.parseArray("/*COMMENT*/  [\"hello\"]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Multi-line comment after opening bracket") {
    JsonArray& arr = jb.parseArray("[/*COMMENT*/ \"hello\"]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Multi-line comment before closing bracket") {
    JsonArray& arr = jb.parseArray("[\"hello\"/*COMMENT*/]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Multi-line comment after closing bracket") {  // parser stopped before encountering the comment
    JsonArray& arr = jb.parseArray("[\"hello\"]/*COMMENT*/");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("Multi-line comment before comma") {
    JsonArray& arr = jb.parseArray("[\"hello\"/*COMMENT*/,\"world\"]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Multi-line comment after comma") {
    JsonArray& arr = jb.parseArray("[\"hello\",/*COMMENT*/ \"world\"]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Single-line comment before opening bracket") {
    JsonArray& arr = jb.parseArray("//COMMENT\n\t[\"hello\"]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Single-line comment after opening bracket") {
    JsonArray& arr = jb.parseArray("[//COMMENT\n\"hello\"]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Single-line comment before closing bracket") {
    JsonArray& arr = jb.parseArray("[\"hello\"//COMMENT\r\n]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Single-line comment after closing bracket") {  // parser stopped before encountering the comment
    JsonArray& arr = jb.parseArray("[\"hello\"]//COMMENT\n");

    REQUIRE(arr.success());
    REQUIRE(1 == arr.size());
    REQUIRE(arr[0] == "hello");
  }

  SECTION("Single-line comment before comma") {
    JsonArray& arr = jb.parseArray("[\"hello\"//COMMENT\n,\"world\"]");
    REQUIRE_FALSE(arr.success());
  }

  SECTION("Single-line comment after comma") {
    JsonArray& arr = jb.parseArray("[\"hello\",//COMMENT\n\"world\"]");
    REQUIRE_FALSE(arr.success());
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
}
