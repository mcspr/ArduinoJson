// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

struct NoMemoryAllocator : ArduinoJson::Allocator {
  void* allocate(size_t) override {
    return nullptr;
  }

  void deallocate(void*) override {
  }

  virtual ~NoMemoryAllocator() {
  }
};

TEST_CASE("DynamicJsonBuffer no memory") {
  NoMemoryAllocator allocator;
  DynamicJsonBuffer _jsonBuffer(&allocator);

  SECTION("FixCodeCoverage") {
    NoMemoryAllocator().deallocate(nullptr);
  }

  SECTION("createArray()") {
    REQUIRE_FALSE(_jsonBuffer.createArray().success());
  }

  SECTION("createObject()") {
    REQUIRE_FALSE(_jsonBuffer.createObject().success());
  }

  SECTION("parseArray()") {
    char json[] = "[]";
    REQUIRE_FALSE(_jsonBuffer.parseArray(json).success());
  }

  SECTION("parseObject()") {
    char json[] = "{}";
    REQUIRE_FALSE(_jsonBuffer.parseObject(json).success());
  }

  SECTION("startString()") {
    auto str = _jsonBuffer.startString();
    str.append('!');
    REQUIRE(nullptr == str.c_str());
  }
}
