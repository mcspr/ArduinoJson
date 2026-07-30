// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

#include "TestAllocators.ipp"

TEST_CASE("DynamicJsonBuffer::parseArray()") {
  OnceAllocator allocator;

  SECTION("RejectAllocateRequest") {
    allocator = OnceAllocator(true);
    DynamicJsonBuffer rejectBuffer(&allocator);
    char input[] = "[]";
    JsonArray& arr = rejectBuffer.parseArray(input);
    REQUIRE_FALSE(arr.success());
  }

  SECTION("CappedAllocatorForAnEmptyArray") {
    CappedAllocator cappedAllocator(0);
    DynamicJsonBuffer cappedBuffer(&cappedAllocator, 0);
    char input[] = "[]";
    JsonArray& arr = cappedBuffer.parseArray(input);
    REQUIRE_FALSE(arr.success());
  }

  SECTION("BufferBlockOverheadFitsEmptyArray") {
    DynamicJsonBuffer bufferOverhead(&allocator, 0);
    char input[] = "[]";
    JsonArray& arr = bufferOverhead.parseArray(input);
    REQUIRE(arr.success());
  }

  SECTION("BufferOfTheRightSizeForEmptyArray") {
    DynamicJsonBuffer bufferOfRightSize(&allocator, JSON_ARRAY_SIZE(0));
    char input[] = "[]";
    JsonArray& arr = bufferOfRightSize.parseArray(input);
    REQUIRE(arr.success());
  }

  SECTION("TooSmallBufferForArrayWithOneValue") {
    DynamicJsonBuffer bufferTooSmall(&allocator, JSON_ARRAY_SIZE(1) - 1);
    char input[] = "[1]";
    JsonArray& arr = bufferTooSmall.parseArray(input);
    REQUIRE_FALSE(arr.success());
  }

  SECTION("BufferOfTheRightSizeForArrayWithOneValue") {
    DynamicJsonBuffer bufferOfRightSize(&allocator, JSON_ARRAY_SIZE(1));
    char input[] = "[1]";
    JsonArray& arr = bufferOfRightSize.parseArray(input);
    REQUIRE(arr.success());
  }

  SECTION("TooSmallBufferForArrayWithNestedObject") {
    DynamicJsonBuffer bufferTooSmall(&allocator, JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(0) - 1);
    char input[] = "[{}]";
    JsonArray& arr = bufferTooSmall.parseArray(input);
    REQUIRE_FALSE(arr.success());
  }

  SECTION("BufferOfTheRightSizeForArrayWithNestedObject") {
    DynamicJsonBuffer bufferOfRightSize(&allocator, JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(0));
    char input[] = "[{}]";
    JsonArray& arr = bufferOfRightSize.parseArray(input);
    REQUIRE(arr.success());
  }

  SECTION("StringDuplicateForcedAlignment") {
    // note: contents depend on current impl, adjust if necessary
    std::string elements[] {
      "123456789ABCDEFG",  // always supposed to allocate
      "ABCD",              // should not allocate w/ 32bit build
    };

    const auto initialSize =
      JSON_ARRAY_SIZE(2) +       // minimal storage requirement
      sizeof(void*) +            // 2nd val variant allocated aligned
      elements[0].length() + 1;  // 1st val contents allocation after 1st variant
    DynamicJsonBuffer jsonBuffer(&allocator, initialSize);

    std::string input;
    input += "  [ \"";
    input += elements[0];
    input += "\", \"";
    input += elements[1];
    input += "\"   ]   ";

    JsonArray& arr = jsonBuffer.parseArray(input);
    REQUIRE(arr.success());
    REQUIRE(arr[0].as<const char*>() == elements[0]);
    REQUIRE(arr[1].as<const char*>() == elements[1]);
    REQUIRE(JSON_ARRAY_SIZE(2) < jsonBuffer.size());
  }
}
