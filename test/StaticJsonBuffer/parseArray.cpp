// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

using StringBufferValue = ArduinoJson::Internals::JsonVariantContent::StringBufferValue;

TEST_CASE("StaticJsonBuffer::parseArray()") {
  SECTION("TooSmallBufferForEmptyArray") {
    StaticJsonBuffer<JSON_ARRAY_SIZE(0) - 1> bufferTooSmall;
    char input[] = "[]";
    JsonArray& arr = bufferTooSmall.parseArray(input);
    REQUIRE_FALSE(arr.success());
  }

  SECTION("BufferOfTheRightSizeForEmptyArray") {
    StaticJsonBuffer<JSON_ARRAY_SIZE(0)> bufferOfRightSize;
    char input[] = "[]";
    JsonArray& arr = bufferOfRightSize.parseArray(input);
    REQUIRE(arr.success());
  }

  SECTION("TooSmallBufferForArrayWithOneValue") {
    StaticJsonBuffer<JSON_ARRAY_SIZE(1) - 1> bufferTooSmall;
    char input[] = "[1]";
    JsonArray& arr = bufferTooSmall.parseArray(input);
    REQUIRE_FALSE(arr.success());
  }

  SECTION("BufferOfTheRightSizeForArrayWithOneValue") {
    StaticJsonBuffer<JSON_ARRAY_SIZE(1)> bufferOfRightSize;
    char input[] = "[1]";
    JsonArray& arr = bufferOfRightSize.parseArray(input);
    REQUIRE(arr.success());
  }

  SECTION("TooSmallBufferForArrayWithNestedObject") {
    StaticJsonBuffer<JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(0) - 1>
        bufferTooSmall;
    char input[] = "[{}]";
    JsonArray& arr = bufferTooSmall.parseArray(input);
    REQUIRE_FALSE(arr.success());
  }

  SECTION("BufferOfTheRightSizeForArrayWithNestedObject") {
    StaticJsonBuffer<JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(0)>
        bufferOfRightSize;
    char input[] = "[{}]";
    JsonArray& arr = bufferOfRightSize.parseArray(input);
    REQUIRE(arr.success());
  }

  SECTION("StringDuplicateForcedAlignment") {
    // note: contents depend on current impl, adjust if necessary
    static constexpr const char* elements[] {
      "123456789ABCDEFG",  // always supposed to allocate
      "ABCD",              // should not allocate w/ 32bit build
    };

    static constexpr auto totalSize = size_t{
      JSON_ARRAY_SIZE(2) +         // minimal storage requirement
      sizeof(void*) +               // 2nd val variant allocated aligned
      __builtin_strlen(elements[0]) + 1  // 1st val variant contents allocation after 1st variant
    };

    StaticJsonBuffer<totalSize> jsonBuffer;

    std::string input;
    input += "  [ \"";
    input += elements[0];
    input += "\", \"";
    input += elements[1];
    input += "\"   ]   ";

    JsonArray& arr = jsonBuffer.parseArray(input);
    REQUIRE(arr.success());
    REQUIRE(arr[0].as<const char*>() == std::string(elements[0]));
    REQUIRE(arr[1].as<const char*>() == std::string(elements[1]));
    REQUIRE(JSON_ARRAY_SIZE(2) < jsonBuffer.size());
  }
}
