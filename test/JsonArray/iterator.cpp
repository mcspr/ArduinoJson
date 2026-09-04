// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

static constexpr const char* nullptr_const_char = nullptr;

template <typename TIterator>
static void run_iterator_test() {
  StaticJsonBuffer<JSON_ARRAY_SIZE(2)> jsonBuffer;

  JsonArray &array = jsonBuffer.createArray();
  array.add(12);
  array.add(34);
  array.add(nullptr_const_char);

  REQUIRE(2 == array.size());

  TIterator it = array.begin();
  TIterator end = array.end();

  REQUIRE(end != it);
  REQUIRE(12 == it->template as<int>());
  REQUIRE(12 == static_cast<int>(*it));
  ++it;
  REQUIRE(end != it);
  REQUIRE(34 == it->template as<int>());
  REQUIRE(34 == static_cast<int>(*it));
  ++it;
  REQUIRE(end == it);
}

TEST_CASE("JsonArray::begin()/end()") {
  SECTION("Mutable") {
    run_iterator_test<JsonArray::iterator>();
  }

  SECTION("Const") {
    run_iterator_test<JsonArray::const_iterator>();
  }
}
