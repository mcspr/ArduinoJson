// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#define ARDUINOJSON_ENABLE_COMMENTS 1
#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonBuffer::parseObject() w/ Comments") {
  DynamicJsonBuffer jb;

  SECTION("CCommentBeforeOpeningBrace") {
    JsonObject& obj = jb.parseObject("/*COMMENT*/  {\"hello\":\"world\"}");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("CCommentAfterOpeningBrace") {
    JsonObject& obj = jb.parseObject("{/*COMMENT*/ \"hello\": \"world\"}");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("CCommentBeforeClosingBrace") {
    JsonObject& obj = jb.parseObject("{\"hello\":\"world\" /*COMMENT*/}");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());

    std::string tmp;
    obj.printTo(tmp);
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("CCommentAfterClosingBrace") {
    JsonObject& obj = jb.parseObject("{\"hello\": \"world\"  }/*COMMENT*/");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("CCommentBeforeComma") {
    JsonObject& obj = jb.parseObject("{\"hello\":\"world\"/*COMMENT*/,\"world\":\"hello\"}");

    REQUIRE(obj.success());
    REQUIRE(2 == obj.size());
    REQUIRE(obj["hello"] == "world");
    REQUIRE(obj["world"] == "hello");
  }

  SECTION("CCommentAfterComma") {
    JsonObject& obj = jb.parseObject("{\"hello\":\"world\",/*COMMENT*/ \"world\":\"hello\"}");

    REQUIRE(obj.success());
    REQUIRE(2 == obj.size());
    REQUIRE(obj["hello"] == "world");
    REQUIRE(obj["world"] == "hello");
  }

  SECTION("CppCommentBeforeOpeningBrace") {
    JsonObject& obj = jb.parseObject("//COMMENT\n\t{\"hello\":\"world\"}");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("CppCommentAfterOpeningBrace") {
    JsonObject& obj = jb.parseObject("{//COMMENT\n\"hello\":\"world\"}");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("CppCommentBeforeClosingBrace") {
    JsonObject& obj = jb.parseObject("{\"hello\":\"world\" //COMMENT\r\n}");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("CppCommentAfterClosingBrace") {
    JsonObject& obj = jb.parseObject("{\"hello\":\"world\"}//COMMENT\n");

    REQUIRE(obj.success());
    REQUIRE(1 == obj.size());
    REQUIRE(obj["hello"] == "world");
  }

  SECTION("CppCommentBeforeComma") {
    JsonObject& obj = jb.parseObject("{\"hello\": \"world\" //COMMENT\n,\"world\": \"hello\"}");

    REQUIRE(obj.success());
    REQUIRE(2 == obj.size());
    REQUIRE(obj["hello"] == "world");
    REQUIRE(obj["world"] == "hello");
  }

  SECTION("CppCommentAfterComma") {
    JsonObject& obj = jb.parseObject("{\"hello\":\"world\",//COMMENT\n\"world\":\"hello\"}");

    REQUIRE(obj.success());
    REQUIRE(2 == obj.size());
    REQUIRE(obj["hello"] == "world");
    REQUIRE(obj["world"] == "hello");
  }

  SECTION("InvalidCppComment") {
    JsonObject& obj = jb.parseObject("{\"hello\":\"world\"/COMMENT\n}");
    REQUIRE_FALSE(obj.success());
  }

  SECTION("InvalidComment") {
    JsonObject& obj = jb.parseObject("{/*/\n}");
    REQUIRE_FALSE(obj.success());
  }

  SECTION("UnfinishedCComment") {
    JsonObject& obj = jb.parseObject("{/*COMMENT}");
    REQUIRE_FALSE(obj.success());
  }

  SECTION("EndsInCppComment") {
    JsonObject& obj = jb.parseObject("{//COMMENT");
    REQUIRE_FALSE(obj.success());
  }

  SECTION("AfterClosingStar") {
    JsonObject& obj = jb.parseObject("{/*COMMENT*");
    REQUIRE_FALSE(obj.success());
  }
}
