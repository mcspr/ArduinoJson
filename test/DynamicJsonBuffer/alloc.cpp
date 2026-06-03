// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>
#include <sstream>

static bool isAligned(void* ptr) {
  const size_t mask = sizeof(void*) - 1;
  size_t addr = reinterpret_cast<size_t>(ptr);
  return (addr & mask) == 0;
}

struct SpyingAllocator : ArduinoJson::Allocator {
  void* allocate(size_t n) override {
    log << "A" << (n - DynamicJsonBuffer::EmptyBlockSize);
    return parent->allocate(n);
  }
  void deallocate(void* p) override {
    log << "F";
    return parent->deallocate(p);
  }

  virtual ~SpyingAllocator() {
  }

  template <typename T>
  static void str(T&& str) {
    log.str(std::forward<T>(str));
  }

  static std::string str() {
    return log.str();
  }

  static std::stringstream log;
  static Allocator* parent;
};

std::stringstream SpyingAllocator::log;
ArduinoJson::Allocator* SpyingAllocator::parent = ArduinoJson::DefaultAllocator::instance();

TEST_CASE("DynamicJsonBuffer::alloc()") {
  SpyingAllocator allocator;

  SECTION("Returns different pointers") {
    DynamicJsonBuffer buffer;
    void* p1 = buffer.alloc(1);
    void* p2 = buffer.alloc(2);
    REQUIRE(p1 != p2);
  }

  SECTION("Doubles allocation size when full") {
    SpyingAllocator::str("");
    {
      DynamicJsonBuffer buffer(&allocator, 1);
      buffer.alloc(1);
      buffer.alloc(1);
    }
    REQUIRE(SpyingAllocator::str() == "A1A2FF");
  }

  SECTION("Resets allocation size after clear()") {
    SpyingAllocator::str("");
    {
      DynamicJsonBuffer buffer(&allocator, 1);
      buffer.alloc(1);
      buffer.alloc(1);
      buffer.clear();
      buffer.alloc(1);
    }
    REQUIRE(SpyingAllocator::str() == "A1A2FFA1F");
  }

  SECTION("Makes a big allocation when needed") {
    SpyingAllocator::str("");
    {
      DynamicJsonBuffer buffer(&allocator, 1);
      buffer.alloc(42);
    }
    REQUIRE(SpyingAllocator::str() == "A42F");
  }

  SECTION("Alignment") {
    // make room for two but not three
    DynamicJsonBuffer tinyBuf(2 * sizeof(void*) + 1);

    REQUIRE(isAligned(tinyBuf.alloc(1)));  // this on is aligned by design
    REQUIRE(isAligned(tinyBuf.alloc(1)));  // this one fits in the first block
    REQUIRE(isAligned(tinyBuf.alloc(1)));  // this one requires a new block
  }
}
