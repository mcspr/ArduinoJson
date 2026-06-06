// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

static bool isAligned(void *ptr) {
  const size_t mask = sizeof(void *) - 1;
  uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
  return (addr & mask) == 0;
}

TEST_CASE("StaticJsonBuffer::alloc()") {
  StaticJsonBuffer<64> buffer;

  SECTION("Returns different addresses") {
    void *p1 = buffer.alloc(1);
    void *p2 = buffer.alloc(1);
    REQUIRE(p1 != p2);
  }

  SECTION("Returns valid pointer when using full capacity") {
    void *p = buffer.alloc(64);
    REQUIRE(nullptr != p);
  }

  SECTION("Returns nullptr when full") {
    buffer.alloc(64);
    void *p = buffer.alloc(1);
    REQUIRE(nullptr == p);
  }

  SECTION("Returns nullptr when buffer is too small") {
    void *p = buffer.alloc(65);
    REQUIRE(nullptr == p);
  }

  SECTION("Returns aligned pointers") {
    for (size_t size = 1; size <= sizeof(void *); size++) {
      void *p = buffer.alloc(1);
      REQUIRE(isAligned(p));
    }
  }

  SECTION("Returns same address after clear()") {
    void *p1 = buffer.alloc(1);
    buffer.clear();
    void *p2 = buffer.alloc(1);
    REQUIRE(p1 == p2);
  }
}
