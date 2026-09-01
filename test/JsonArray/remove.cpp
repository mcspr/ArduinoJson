// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonArray::remove()") {
  DynamicJsonBuffer _jsonBuffer;
  JsonArray& _array = _jsonBuffer.createArray();
  _array.add(1);
  _array.add(2);
  _array.add(3);
  _array.add(4);

  SECTION("RemoveFirstByIndex") {
    _array.remove(0);

    REQUIRE(3 == _array.size());
    REQUIRE(_array[0] == 2);
    REQUIRE(_array[1] == 3);
    REQUIRE(_array[2] == 4);
  }

  SECTION("RemoveMiddleByIndex") {
    _array.remove(1);
    _array.remove(1);

    REQUIRE(2 == _array.size());
    REQUIRE(_array[0] == 1);
    REQUIRE(_array[1] == 4);
  }

  SECTION("RemoveLastByIndex") {
    _array.remove(_array.size() - 1);

    REQUIRE(3 == _array.size());
    REQUIRE(_array[0] == 1);
    REQUIRE(_array[1] == 2);
    REQUIRE(_array[2] == 3);
  }

  SECTION("RemoveByIterator") {
    for (auto it = _array.begin(); it != _array.end(); ++it) {
      if ((*it).is<int>() && (*it).as<int>() == 2) {
        it = _array.remove(it);
        it = _array.remove(it);
      }
    }

    REQUIRE(2 == _array.size());
    REQUIRE(_array[0] == 1);
    REQUIRE(_array[1] == 4);
  }

  SECTION("RemoveFirstByIterator") {
    auto it = _array.begin();
    it = _array.remove(it);

    REQUIRE(it != _array.end());
    REQUIRE((*it) == 2);

    REQUIRE(3 == _array.size());
    REQUIRE(_array[0] == 2);
    REQUIRE(_array[1] == 3);
    REQUIRE(_array[2] == 4);
  }

  SECTION("RemoveMiddleByIterator") {
    auto it = _array.begin();
    it = _array.remove(it + 1);
    it = _array.remove(it);

    REQUIRE(it != _array.end());
    REQUIRE((*it) == 4);

    REQUIRE(2 == _array.size());
    REQUIRE(_array[0] == 1);
    REQUIRE(_array[1] == 4);
  }

  SECTION("RemoveLastByIterator") {
    auto it = _array.begin();
    it = _array.remove(it + (_array.size() - 1));

    REQUIRE(it == _array.end());

    REQUIRE(3 == _array.size());
    REQUIRE(_array[0] == 1);
    REQUIRE(_array[1] == 2);
    REQUIRE(_array[2] == 3);
  }
}
