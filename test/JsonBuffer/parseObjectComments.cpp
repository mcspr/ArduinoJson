// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#define ARDUINOJSON_ENABLE_COMMENTS 1
#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonBuffer::parseObject() w/ Comments") {
  DynamicJsonBuffer jb;

  SECTION("Multi-line comment before opening brace") {
    JsonObject& obj = jb.parseObject("/*COMMENT*/  {\"hello\":\"world\"}");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("Multi-line comment after opening brace") {
    JsonObject& obj = jb.parseObject("{/*COMMENT*/ \"hello\": \"world\"}");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("Multi-line comment before closing brace") {
    JsonObject& obj = jb.parseObject("{\"hello\":\"world\" /*COMMENT*/}");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());

    std::string tmp;
    obj.printTo(tmp);
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("Multi-line comment after closing brace") {  // parser stopped before encountering the comment
    JsonObject& obj = jb.parseObject("{\"hello\": \"world\"  }/*COMMENT*/");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("Multi-line comment before comma") {
    JsonObject& obj = jb.parseObject("{\"hello\":\"world\"/*COMMENT*/,\"world\":\"hello\"}");

    REQUIRE(obj.success());
    REQUIRE(2 == obj.size());
    REQUIRE(obj["hello"] == "world");
    REQUIRE(obj["world"] == "hello");
  }

  SECTION("Multi-line comment after comma") {
    JsonObject& obj = jb.parseObject("{\"hello\":\"world\",/*COMMENT*/ \"world\":\"hello\"}");

    REQUIRE(obj.success());
    REQUIRE(2 == obj.size());
    REQUIRE(obj["hello"] == "world");
    REQUIRE(obj["world"] == "hello");
  }

  SECTION("Single-line comment before opening brace") {
    JsonObject& obj = jb.parseObject("//COMMENT\n\t{\"hello\":\"world\"}");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("Single-line comment after opening brace") {
    JsonObject& obj = jb.parseObject("{//COMMENT\n\"hello\":\"world\"}");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("Single-line comment before closing brace") {
    JsonObject& obj = jb.parseObject("{\"hello\":\"world\" //COMMENT\r\n}");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("Single-line comment after closing brace") {
    JsonObject& obj = jb.parseObject("{\"hello\":\"world\"}//COMMENT\n");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("Single-line comment before comma") {
    JsonObject& obj = jb.parseObject("{\"hello\": \"world\" //COMMENT\n,\"world\": \"hello\"}");

    REQUIRE(obj.success());
    REQUIRE(2 == obj.size());
    REQUIRE(obj["hello"] == "world");
    REQUIRE(obj["world"] == "hello");
  }

  SECTION("Single-line comment after comma") {
    JsonObject& obj = jb.parseObject("{\"hello\":\"world\",//COMMENT\n\"world\":\"hello\"}");

    REQUIRE(obj.success());
    REQUIRE(2 == obj.size());
    REQUIRE(obj["hello"] == "world");
    REQUIRE(obj["world"] == "hello");
  }

  SECTION("Invalid single-line comment") {
    JsonObject& obj = jb.parseObject("{\"hello\":\"world\"/COMMENT\n}");
    REQUIRE_FALSE(obj.success());
  }

  SECTION("Invalid multi-line comment") {
    JsonObject& obj = jb.parseObject("{/*/\n}");
    REQUIRE_FALSE(obj.success());
  }

  SECTION("Unfinished multi-line comment") {
    JsonObject& obj = jb.parseObject("{/*COMMENT}");
    REQUIRE_FALSE(obj.success());
  }

  SECTION("Ends in single-line comment") {
    JsonObject& obj = jb.parseObject("{//COMMENT");
    REQUIRE_FALSE(obj.success());
  }

  SECTION("After closing star") {
    JsonObject& obj = jb.parseObject("{/*COMMENT*");
    REQUIRE_FALSE(obj.success());
  }
}
