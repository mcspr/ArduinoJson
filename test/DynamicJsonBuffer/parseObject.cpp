// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

#include "TestAllocators.ipp"

TEST_CASE("DynamicJsonBuffer::parseObject()") {
  OnceAllocator allocator;

  SECTION("RejectAllocateRequest") {
    allocator = OnceAllocator(true);
    DynamicJsonBuffer rejectBuffer(&allocator);
    char input[] = "{}";
    JsonObject& obj = rejectBuffer.parseObject(input);
    REQUIRE_FALSE(obj.success());
  }

  SECTION("CappedAllocatorForAnEmptyObject") {
    CappedAllocator cappedAllocator(0);
    DynamicJsonBuffer cappedBuffer(&cappedAllocator, 0);
    char input[] = "{}";
    JsonObject& obj = cappedBuffer.parseObject(input);
    REQUIRE_FALSE(obj.success());
  }

  SECTION("BufferBlockOverheadFitsEmptyObject") {
    DynamicJsonBuffer bufferOverhead(&allocator, 0);
    char input[] = "{}";
    JsonObject& obj = bufferOverhead.parseObject(input);
    REQUIRE(obj.success());
  }

  SECTION("BufferOfTheRightSizeForEmptyObject") {
    DynamicJsonBuffer bufferOfRightSize(&allocator, JSON_OBJECT_SIZE(0));
    char input[] = "{}";
    JsonObject& obj = bufferOfRightSize.parseObject(input);
    REQUIRE(obj.success());
  }

  SECTION("TooSmallBufferForObjectWithOneValue") {
    DynamicJsonBuffer bufferTooSmall(&allocator, JSON_OBJECT_SIZE(1) - 1);
    char input[] = "{\"a\":1}";
    JsonObject& obj = bufferTooSmall.parseObject(input);
    REQUIRE_FALSE(obj.success());
  }

  SECTION("BufferOfTheRightSizeForObjectWithOneValue") {
    DynamicJsonBuffer bufferOfRightSize(&allocator, JSON_OBJECT_SIZE(1));
    char input[] = "{\"a\":1}";
    JsonObject& obj = bufferOfRightSize.parseObject(input);
    REQUIRE(obj.success());
  }

  SECTION("TooSmallBufferForObjectWithNestedObject") {
    DynamicJsonBuffer bufferTooSmall(&allocator, JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(0) - 1);
    char input[] = "{\"a\":[]}";
    JsonObject& obj = bufferTooSmall.parseObject(input);
    REQUIRE_FALSE(obj.success());
  }

  SECTION("BufferOfTheRightSizeForObjectWithNestedObject") {
    DynamicJsonBuffer bufferOfRightSize(&allocator, JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(0));
    char input[] = "{\"a\":[]}";
    JsonObject& obj = bufferOfRightSize.parseObject(input);
    REQUIRE(obj.success());
  }

  SECTION("StringDuplicateForcedAlignment") {
    // note: contents depend on current impl, adjust if necessary
    std::string elements[] {
      "123456789ABCDEFG",  // always supposed to allocate
      "ABCD",              // should not allocate w/ 32bit build
    };

    const auto initialSize =
      JSON_OBJECT_SIZE(2) +      // minimal storage requirement
      sizeof(void*) +            // 2nd kv pair allocated aligned
      elements[0].length() + 1;  // 1st val allocation after 1st kv pair
    DynamicJsonBuffer jsonBuffer(&allocator, initialSize);

    std::string input;
    input += "  { key1: \"";
    input += elements[0];
    input += "\", key2: \"";
    input += elements[1];
    input += "\"   }   ";

    JsonObject& obj = jsonBuffer.parseObject(input);
    REQUIRE(obj.success());
    REQUIRE(obj["key1"].as<const char*>() == elements[0]);
    REQUIRE(obj["key2"].as<const char*>() == elements[1]);
    REQUIRE(JSON_OBJECT_SIZE(2) < jsonBuffer.size());
  }
}
