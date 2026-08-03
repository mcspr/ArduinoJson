// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonArray::operator[]") {
  DynamicJsonBuffer _jsonBuffer;
  JsonArray& _array = _jsonBuffer.createArray();
  _array.add(0);

  SECTION("char") {
    _array[0] = 1;

    REQUIRE(_array[0].is<char>());
    REQUIRE(_array[0].is<short>());
    REQUIRE(_array[0].is<int>());
    REQUIRE(_array[0].is<long>());
    REQUIRE(1 == _array[0].as<char>());
    REQUIRE(1 == _array[0].as<short>());
    REQUIRE(1 == _array[0].as<int>());
    REQUIRE(1 == _array[0].as<long>());

    REQUIRE_FALSE(_array[0].is<bool>());
  }

  SECTION("short") {
    _array[0] = 1234;

    REQUIRE(_array[0].is<char>());
    REQUIRE(_array[0].is<short>());
    REQUIRE(_array[0].is<int>());
    REQUIRE(_array[0].is<long>());

    REQUIRE(0 == _array[0].as<char>());
    REQUIRE(1234 == _array[0].as<short>());
    REQUIRE(1234 == _array[0].as<int>());
    REQUIRE(1234 == _array[0].as<long>());

    REQUIRE_FALSE(_array[0].is<bool>());
  }

  SECTION("int") {
    _array[0] = 123456;

    REQUIRE(_array[0].is<char>());
    REQUIRE(_array[0].is<short>());
    REQUIRE(_array[0].is<int>());
    REQUIRE(_array[0].is<long>());

    REQUIRE(0 == _array[0].as<char>());
    REQUIRE(0 == _array[0].as<short>());
    REQUIRE(123456 == _array[0].as<int>());
    REQUIRE(123456 == _array[0].as<long>());

    REQUIRE_FALSE(_array[0].is<bool>());
  }

#if ARDUINOJSON_USE_LONG_LONG || ARDUINOJSON_USE_INT64
  SECTION("long long") {
    static constexpr long long value = std::numeric_limits<long long>::max();
    _array[0] = value;

    REQUIRE(_array[0].is<char>());
    REQUIRE(_array[0].is<short>());
    REQUIRE(_array[0].is<int>());
    REQUIRE(_array[0].is<long>());

    REQUIRE(0 == _array[0].as<char>());
    REQUIRE(0 == _array[0].as<short>());
    REQUIRE(0 == _array[0].as<int>());
    REQUIRE(value == _array[0].as<long long>());

    REQUIRE_FALSE(_array[0].is<bool>());
  }
#endif

  SECTION("double") {
    _array[0] = 123.45;

    REQUIRE(_array[0].is<double>());
    REQUIRE(Approx(123.45) == _array[0].as<double>());

    REQUIRE_FALSE(_array[0].is<char>());
    REQUIRE_FALSE(_array[0].is<short>());
    REQUIRE_FALSE(_array[0].is<int>());
    REQUIRE_FALSE(_array[0].is<long>());
  }

  SECTION("bool") {
    _array[0] = true;

    REQUIRE(_array[0].is<bool>());
    REQUIRE(_array[0].as<bool>());

    REQUIRE_FALSE(_array[0].is<char>());
    REQUIRE_FALSE(_array[0].is<short>());
    REQUIRE_FALSE(_array[0].is<int>());
    REQUIRE_FALSE(_array[0].is<long>());
  }

  SECTION("const char*") {
    const char* str = "hello";

    _array[0] = str;
    REQUIRE(str == _array[0].as<const char*>());
    REQUIRE(str == _array[0].as<char*>());  // <- short hand
    REQUIRE(true == _array[0].is<const char*>());

    REQUIRE_FALSE(_array[0].is<char>());
    REQUIRE_FALSE(_array[0].is<short>());
    REQUIRE_FALSE(_array[0].is<int>());
    REQUIRE_FALSE(_array[0].is<long>());
  }

  SECTION("nested array") {
    JsonArray& arr = _jsonBuffer.createArray();

    _array[0] = arr;

    REQUIRE(std::addressof(arr) == std::addressof(_array[0].as<JsonArray&>()));
    REQUIRE(std::addressof(arr) == std::addressof(_array[0].as<JsonArray>()));  // <- short hand
    REQUIRE(std::addressof(arr) == std::addressof(_array[0].as<const JsonArray&>()));
    REQUIRE(std::addressof(arr) == std::addressof(_array[0].as<const JsonArray>()));  // short hand
    REQUIRE(_array[0].is<JsonArray&>());

    REQUIRE_FALSE(_array[0].is<char>());
    REQUIRE_FALSE(_array[0].is<short>());
    REQUIRE_FALSE(_array[0].is<int>());
    REQUIRE_FALSE(_array[0].is<long>());
  }

  SECTION("nested object") {
    JsonObject& obj = _jsonBuffer.createObject();

    _array[0] = obj;

    REQUIRE(std::addressof(obj) == std::addressof(_array[0].as<JsonObject&>()));
    REQUIRE(std::addressof(obj) == std::addressof(_array[0].as<JsonObject>()));  // <- short hand
    REQUIRE(std::addressof(obj) == std::addressof(_array[0].as<const JsonObject&>()));
    REQUIRE(std::addressof(obj) == std::addressof(_array[0].as<const JsonObject>()));  // <- short hand
    REQUIRE(_array[0].is<JsonObject&>());

    REQUIRE_FALSE(_array[0].is<char>());
    REQUIRE_FALSE(_array[0].is<short>());
    REQUIRE_FALSE(_array[0].is<int>());
    REQUIRE_FALSE(_array[0].is<long>());
  }

  SECTION("array subscript assignment") {
    JsonArray& arr = _jsonBuffer.createArray();
    const char* str = "hello";

    arr.add(str);

    _array[0] = arr[0];

    REQUIRE(str == _array[0]);
  }

  SECTION("array subscript move") {
    JsonArray& arr = _jsonBuffer.createArray();
    const char* str = "hello";

    arr.add(str);

    auto sub = arr[0];
    _array[0] = std::move(sub);

    REQUIRE(str == _array[0]);
  }

  SECTION("object subscript assignment") {
    JsonObject& obj = _jsonBuffer.createObject();
    const char* str = "hello";

    obj["x"] = str;

    _array[0] = obj["x"];

    REQUIRE(str == _array[0]);
  }

  SECTION("object subscript move") {
    JsonObject& obj = _jsonBuffer.createObject();
    const char* str = "hello";

    obj["x"] = str;

    auto sub = obj["x"];
    _array[0] = std::move(sub);

    REQUIRE(str == _array[0]);
  }

  SECTION("should not duplicate const char*") {
    _array[0] = "world";
    const size_t expectedSize = JSON_ARRAY_SIZE(1);
    REQUIRE(expectedSize == _jsonBuffer.size());
  }

  SECTION("should use variant string buffer for short char*") {
    _array[0] = const_cast<char*>("world");
    const size_t expectedSize = JSON_ARRAY_SIZE(1);
    REQUIRE(expectedSize == _jsonBuffer.size());
  }

  SECTION("should duplicate long char*") {
    const char* val = "thisstringisaprettybigonetofitintothevariantstringbuffer";
    _array[0] = const_cast<char*>(val);
    const size_t expectedSize = JSON_ARRAY_SIZE(1) + strlen(val) + 1;
    REQUIRE(expectedSize == _jsonBuffer.size());
  }

  SECTION("should use variant string buffer for short std::string") {
    _array[0] = std::string("world");
    const size_t expectedSize = JSON_ARRAY_SIZE(1);
    REQUIRE(expectedSize == _jsonBuffer.size());
  }

  SECTION("should duplicate long std::string") {
    std::string val = "thisstringistoolongforthisarray";
    _array[0] = val;
    const size_t expectedSize = JSON_ARRAY_SIZE(1) + val.length() + 1;
    REQUIRE(expectedSize == _jsonBuffer.size());
  }
}
