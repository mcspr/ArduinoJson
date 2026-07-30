// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>
TEST_CASE("StaticJsonBuffer::parseObject()") {
  SECTION("TooSmallBufferForEmptyObject") {
    StaticJsonBuffer<JSON_OBJECT_SIZE(0) - 1> bufferTooSmall;
    char input[] = "{}";
    JsonObject& obj = bufferTooSmall.parseObject(input);
    REQUIRE_FALSE(obj.success());
  }

  SECTION("BufferOfTheRightSizeForEmptyObject") {
    StaticJsonBuffer<JSON_OBJECT_SIZE(0)> bufferOfRightSize;
    char input[] = "{}";
    JsonObject& obj = bufferOfRightSize.parseObject(input);
    REQUIRE(obj.success());
  }

  SECTION("TooSmallBufferForObjectWithOneValue") {
    StaticJsonBuffer<JSON_OBJECT_SIZE(1) - 1> bufferTooSmall;
    char input[] = "{\"a\":1}";
    JsonObject& obj = bufferTooSmall.parseObject(input);
    REQUIRE_FALSE(obj.success());
  }

  SECTION("BufferOfTheRightSizeForObjectWithOneValue") {
    StaticJsonBuffer<JSON_OBJECT_SIZE(1)> bufferOfRightSize;
    char input[] = "{\"a\":1}";
    JsonObject& obj = bufferOfRightSize.parseObject(input);
    REQUIRE(obj.success());
  }

  SECTION("TooSmallBufferForObjectWithNestedObject") {
    StaticJsonBuffer<JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(0) - 1>
        bufferTooSmall;
    char input[] = "{\"a\":[]}";
    JsonObject& obj = bufferTooSmall.parseObject(input);
    REQUIRE_FALSE(obj.success());
  }

  SECTION("BufferOfTheRightSizeForObjectWithNestedObject") {
    StaticJsonBuffer<JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(0)>
        bufferOfRightSize;
    char input[] = "{\"a\":[]}";
    JsonObject& obj = bufferOfRightSize.parseObject(input);
    REQUIRE(obj.success());
  }

  SECTION("StringDuplicateForcedAlignment") {
    // note: contents depend on current impl, adjust if necessary
    static constexpr const char* elements[] {
      "123456789ABCDEFG",  // always supposed to allocate
      "ABCD",              // should not allocate w/ 32bit build
    };

    static constexpr auto totalSize = size_t{
      JSON_OBJECT_SIZE(2) +         // minimal storage requirement
      sizeof(void*) +               // 2nd val variant allocated aligned
      __builtin_strlen(elements[0]) + 1  // 1st val variant contents allocation after 1st variant
    };

    StaticJsonBuffer<totalSize> bufferOfRightSize;

    std::string input;
    input += "  { key1: \"";
    input += elements[0];
    input += "\", key2: \"";
    input += elements[1];
    input += "\"   }   ";

    JsonObject& obj = bufferOfRightSize.parseObject(input);
    REQUIRE(obj.success());
    REQUIRE(obj["key1"].as<const char*>() == std::string(elements[0]));
    REQUIRE(obj["key2"].as<const char*>() == std::string(elements[1]));
    REQUIRE(JSON_OBJECT_SIZE(2) < bufferOfRightSize.size());
  }
}
