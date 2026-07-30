// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>
#include <sstream>

#include "TestAllocators.ipp"

static std::stringstream& logger() {
  static std::stringstream instance;
  return instance;
}

std::string loggerOutput() {
  auto& instance = logger();
  std::string out = instance.str();
  instance.str("");
  return out;
}

static inline bool isAligned(void* ptr) {
  const size_t mask = sizeof(void*) - 1;
  size_t addr = reinterpret_cast<size_t>(ptr);
  return (addr & mask) == 0;
}

TEST_CASE("DynamicJsonBuffer::alloc()") {
  SpyingAllocator allocator(logger());

  SECTION("Returns different pointers") {
    DynamicJsonBuffer buffer;
    void* p1 = buffer.alloc(1);
    void* p2 = buffer.alloc(2);
    REQUIRE(p1 != p2);
  }

  SECTION("Doubles allocation size when full") {
    {
      DynamicJsonBuffer buffer(&allocator, 1);
      buffer.alloc(1);
      buffer.alloc(1);
    }
    REQUIRE(loggerOutput() == "A1A2FF");
  }

  SECTION("Resets allocation size after clear()") {
    {
      DynamicJsonBuffer buffer(&allocator, 1);
      buffer.alloc(1);
      buffer.alloc(1);
      buffer.clear();
      buffer.alloc(1);
    }
    REQUIRE(loggerOutput() == "A1A2FFA1F");
  }

  SECTION("Makes a big allocation when needed") {
    {
      DynamicJsonBuffer buffer(&allocator, 1);
      buffer.alloc(42);
    }
    REQUIRE(loggerOutput() == "A42F");
  }

  SECTION("Alignment") {
    // make room for two but not three
    DynamicJsonBuffer tinyBuf(2 * sizeof(void*) + 1);

    REQUIRE(isAligned(tinyBuf.alloc(1)));  // initial allocation is aligned
    REQUIRE(isAligned(tinyBuf.alloc(1)));  // fits into the first block
    REQUIRE(isAligned(tinyBuf.alloc(1)));  // requires a new block
  }
}
