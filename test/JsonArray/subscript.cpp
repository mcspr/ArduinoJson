// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonArray::operator[]") {
  DynamicJsonBuffer _jsonBuffer;
  JsonArray& _array = _jsonBuffer.createArray();
  _array.add(9876);

  SECTION("char") {
    signed char value = 12;
    _array[0] = value;

    REQUIRE(_array[0].is<char>());
    REQUIRE(_array[0].is<signed char>());
    REQUIRE(_array[0].is<short>());
    REQUIRE(_array[0].is<int>());
    REQUIRE(_array[0].is<long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(_array[0].is<long long>());
#endif

    REQUIRE(12 == _array[0].as<char>());
    REQUIRE(12 == _array[0].as<short>());
    REQUIRE(12 == _array[0].as<int>());
    REQUIRE(12 == _array[0].as<long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(12 == _array[0].as<long long>());
#endif

    REQUIRE_FALSE(_array[0].is<bool>());
  }

  SECTION("short") {
    short value = 12345;
    _array[0] = value;

    REQUIRE(_array[0].is<char>());
    REQUIRE(_array[0].is<short>());
    REQUIRE(_array[0].is<int>());
    REQUIRE(_array[0].is<long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(_array[0].is<long long>());
#endif

    REQUIRE(0 == _array[0].as<char>());
    REQUIRE(12345 == _array[0].as<short>());
    REQUIRE(12345 == _array[0].as<int>());
    REQUIRE(12345 == _array[0].as<long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(12345 == _array[0].as<long long>());
#endif

    REQUIRE_FALSE(_array[0].is<bool>());
  }

  SECTION("int") {
    int value = -123456;
    _array[0] = value;

    REQUIRE(_array[0].is<char>());
    REQUIRE(_array[0].is<short>());
    REQUIRE(_array[0].is<int>());
    REQUIRE(_array[0].is<long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(_array[0].is<long long>());
#endif

    REQUIRE(0 == _array[0].as<char>());
    REQUIRE(0 == _array[0].as<short>());
    REQUIRE(-123456 == _array[0].as<int>());
    REQUIRE(-123456 == _array[0].as<long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(-123456 == _array[0].as<long long>());
#endif

    REQUIRE_FALSE(_array[0].is<bool>());
  }

  SECTION("long") {
    long value = 123456;
    _array[0] = value;

    REQUIRE(_array[0].is<char>());
    REQUIRE(_array[0].is<short>());
    REQUIRE(_array[0].is<int>());
    REQUIRE(_array[0].is<long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(_array[0].is<long long>());
#endif

    REQUIRE(0 == _array[0].as<char>());
    REQUIRE(0 == _array[0].as<short>());
    REQUIRE(123456 == _array[0].as<int>());
    REQUIRE(123456 == _array[0].as<long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(123456 == _array[0].as<long long>());
#endif

    REQUIRE_FALSE(_array[0].is<bool>());
  }

#if ARDUINOJSON_USE_LONG_LONG
  SECTION("long long") {
    static constexpr long long value = std::numeric_limits<long long>::max();
    _array[0] = value;

    REQUIRE(_array[0].is<char>());
    REQUIRE(_array[0].is<short>());
    REQUIRE(_array[0].is<int>());
    REQUIRE(_array[0].is<long>());
    REQUIRE(_array[0].is<long long>());

    REQUIRE(0 == _array[0].as<char>());
    REQUIRE(0 == _array[0].as<short>());
    REQUIRE(0 == _array[0].as<int>());
    REQUIRE(value == _array[0].as<long long>());

    REQUIRE_FALSE(_array[0].is<bool>());
  }
#endif

  SECTION("unsigned char") {
    unsigned char value = 123;
    _array[0] = value;

    REQUIRE(_array[0].is<unsigned char>());
    REQUIRE(_array[0].is<unsigned short>());
    REQUIRE(_array[0].is<unsigned int>());
    REQUIRE(_array[0].is<unsigned long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(_array[0].is<unsigned long long>());
#endif

    REQUIRE(123 == _array[0].as<unsigned char>());
    REQUIRE(123 == _array[0].as<unsigned short>());
    REQUIRE(123 == _array[0].as<unsigned int>());
    REQUIRE(123 == _array[0].as<unsigned long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(123 == _array[0].as<unsigned long long>());
#endif

    REQUIRE_FALSE(_array[0].is<bool>());
  }

  SECTION("unsigned short") {
    unsigned short value = 1234;
    _array[0] = value;

    REQUIRE(_array[0].is<unsigned char>());
    REQUIRE(_array[0].is<unsigned short>());
    REQUIRE(_array[0].is<unsigned int>());
    REQUIRE(_array[0].is<unsigned long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(_array[0].is<unsigned long long>());
#endif

    REQUIRE(0 == _array[0].as<unsigned char>());
    REQUIRE(1234 == _array[0].as<unsigned short>());
    REQUIRE(1234 == _array[0].as<unsigned int>());
    REQUIRE(1234 == _array[0].as<unsigned long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(1234 == _array[0].as<unsigned long long>());
#endif

    REQUIRE_FALSE(_array[0].is<bool>());
  }

  SECTION("unsigned int") {
    unsigned int value = 123456;
    _array[0] = value;

    REQUIRE(_array[0].is<unsigned char>());
    REQUIRE(_array[0].is<unsigned short>());
    REQUIRE(_array[0].is<unsigned int>());
    REQUIRE(_array[0].is<unsigned long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(_array[0].is<unsigned long long>());
#endif

    REQUIRE(0 == _array[0].as<unsigned char>());
    REQUIRE(0 == _array[0].as<unsigned short>());
    REQUIRE(123456 == _array[0].as<unsigned int>());
    REQUIRE(123456 == _array[0].as<unsigned long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(123456 == _array[0].as<unsigned long long>());
#endif

    REQUIRE_FALSE(_array[0].is<bool>());
  }

  SECTION("unsigned long") {
    unsigned long value = 123456;
    _array[0] = value;

    REQUIRE(_array[0].is<unsigned char>());
    REQUIRE(_array[0].is<unsigned short>());
    REQUIRE(_array[0].is<unsigned int>());
    REQUIRE(_array[0].is<unsigned long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(_array[0].is<unsigned long long>());
#endif

    REQUIRE(0 == _array[0].as<unsigned char>());
    REQUIRE(0 == _array[0].as<unsigned short>());
    REQUIRE(123456 == _array[0].as<unsigned int>());
    REQUIRE(123456 == _array[0].as<unsigned long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE(123456 == _array[0].as<unsigned long long>());
#endif

    REQUIRE_FALSE(_array[0].is<bool>());
  }

#if ARDUINOJSON_USE_LONG_LONG
  SECTION("unsigned long long") {
    static constexpr auto value =
      std::numeric_limits<unsigned long long>::max();
    _array[0] = value;

    REQUIRE(_array[0].is<unsigned char>());
    REQUIRE(_array[0].is<unsigned short>());
    REQUIRE(_array[0].is<unsigned int>());
    REQUIRE(_array[0].is<unsigned long>());
    REQUIRE(_array[0].is<unsigned long long>());

    REQUIRE(0 == _array[0].as<unsigned char>());
    REQUIRE(0 == _array[0].as<unsigned short>());
    REQUIRE(0 == _array[0].as<unsigned int>());
    REQUIRE(value == _array[0].as<unsigned long long>());

    REQUIRE_FALSE(_array[0].is<bool>());
  }
#endif

  SECTION("float") {
    _array[0] = 123.45f;

    REQUIRE(_array[0].is<float>());
    REQUIRE(_array[0].is<double>());
    REQUIRE(Approx(123.45f) == _array[0].as<float>());

    REQUIRE_FALSE(_array[0].is<char>());
    REQUIRE_FALSE(_array[0].is<short>());
    REQUIRE_FALSE(_array[0].is<int>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE_FALSE(_array[0].is<long long>());
#endif

    REQUIRE_FALSE(_array[0].is<unsigned char>());
    REQUIRE_FALSE(_array[0].is<unsigned short>());
    REQUIRE_FALSE(_array[0].is<unsigned int>());
    REQUIRE_FALSE(_array[0].is<unsigned long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE_FALSE(_array[0].is<unsigned long long>());
#endif
  }

  SECTION("double") {
    _array[0] = 123.45;

    REQUIRE(_array[0].is<float>());
    REQUIRE(_array[0].is<double>());
    REQUIRE(Approx(123.45) == _array[0].as<double>());

    REQUIRE_FALSE(_array[0].is<char>());
    REQUIRE_FALSE(_array[0].is<short>());
    REQUIRE_FALSE(_array[0].is<int>());
    REQUIRE_FALSE(_array[0].is<long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE_FALSE(_array[0].is<long long>());
#endif

    REQUIRE_FALSE(_array[0].is<unsigned char>());
    REQUIRE_FALSE(_array[0].is<unsigned short>());
    REQUIRE_FALSE(_array[0].is<unsigned int>());
    REQUIRE_FALSE(_array[0].is<unsigned long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE_FALSE(_array[0].is<unsigned long long>());
#endif
  }

  SECTION("bool") {
    _array[0] = true;

    REQUIRE(_array[0].is<bool>());
    REQUIRE(_array[0].as<bool>());

    REQUIRE_FALSE(_array[0].is<char>());
    REQUIRE_FALSE(_array[0].is<short>());
    REQUIRE_FALSE(_array[0].is<int>());
    REQUIRE_FALSE(_array[0].is<long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE_FALSE(_array[0].is<long long>());
#endif

    REQUIRE_FALSE(_array[0].is<unsigned char>());
    REQUIRE_FALSE(_array[0].is<unsigned short>());
    REQUIRE_FALSE(_array[0].is<unsigned int>());
    REQUIRE_FALSE(_array[0].is<unsigned long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE_FALSE(_array[0].is<unsigned long long>());
#endif
  }

  SECTION("const char*") {
    const char* str = "hello";

    _array[0] = str;
    REQUIRE(str == _array[0].as<const char*>());
    REQUIRE(str == _array[0].as<char*>());  // <- shorthand
    REQUIRE(_array[0].is<const char*>());

    REQUIRE_FALSE(_array[0].is<char>());
    REQUIRE_FALSE(_array[0].is<short>());
    REQUIRE_FALSE(_array[0].is<int>());
    REQUIRE_FALSE(_array[0].is<long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE_FALSE(_array[0].is<long long>());
#endif

    REQUIRE_FALSE(_array[0].is<unsigned char>());
    REQUIRE_FALSE(_array[0].is<unsigned short>());
    REQUIRE_FALSE(_array[0].is<unsigned int>());
    REQUIRE_FALSE(_array[0].is<unsigned long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE_FALSE(_array[0].is<unsigned long long>());
#endif
  }

  SECTION("nested array") {
    JsonArray& arr = _jsonBuffer.createArray();
    arr.add(42);

    _array[0] = arr;

    REQUIRE(std::addressof(arr) == std::addressof(_array[0].as<JsonArray&>()));
    REQUIRE(std::addressof(arr) == std::addressof(_array[0].as<JsonArray>()));  // <- shorthand
    REQUIRE(std::addressof(arr) == std::addressof(_array[0].as<const JsonArray&>()));
    REQUIRE(std::addressof(arr) == std::addressof(_array[0].as<const JsonArray>()));  // shorthand

    REQUIRE_FALSE(_array[0].is<char>());
    REQUIRE_FALSE(_array[0].is<short>());
    REQUIRE_FALSE(_array[0].is<int>());
    REQUIRE_FALSE(_array[0].is<long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE_FALSE(_array[0].is<long long>());
#endif

    REQUIRE_FALSE(_array[0].is<unsigned char>());
    REQUIRE_FALSE(_array[0].is<unsigned short>());
    REQUIRE_FALSE(_array[0].is<unsigned int>());
    REQUIRE_FALSE(_array[0].is<unsigned long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE_FALSE(_array[0].is<unsigned long long>());
#endif

    REQUIRE(_array[0].is<JsonArray&>());
    REQUIRE(_array[0][0].is<int>());
    REQUIRE(42 == _array[0][0]);
  }

  SECTION("nested object") {
    JsonObject& obj = _jsonBuffer.createObject();
    obj["x"] = 42;

    _array[0] = obj;

    REQUIRE(std::addressof(obj) == std::addressof(_array[0].as<JsonObject&>()));
    REQUIRE(std::addressof(obj) == std::addressof(_array[0].as<JsonObject>()));  // <- shorthand
    REQUIRE(std::addressof(obj) == std::addressof(_array[0].as<const JsonObject&>()));
    REQUIRE(std::addressof(obj) == std::addressof(_array[0].as<const JsonObject>()));  // <- shorthand

    REQUIRE_FALSE(_array[0].is<char>());
    REQUIRE_FALSE(_array[0].is<short>());
    REQUIRE_FALSE(_array[0].is<int>());
    REQUIRE_FALSE(_array[0].is<long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE_FALSE(_array[0].is<long long>());
#endif

    REQUIRE_FALSE(_array[0].is<unsigned char>());
    REQUIRE_FALSE(_array[0].is<unsigned short>());
    REQUIRE_FALSE(_array[0].is<unsigned int>());
    REQUIRE_FALSE(_array[0].is<unsigned long>());
#if ARDUINOJSON_USE_LONG_LONG
    REQUIRE_FALSE(_array[0].is<unsigned long long>());
#endif

    REQUIRE(_array[0].is<JsonObject&>());
    REQUIRE(_array[0]["x"].is<int>());
    REQUIRE(42 == _array[0]["x"]);
  }

  SECTION("array subscript assignment") {
    JsonArray& arr = _jsonBuffer.createArray();
    const char* str = "hello";

    arr.add(str);
    _array[0] = arr[0];

    REQUIRE(str == _array[0]);
  }

  SECTION("array subscript set") {
    JsonArray& arr = _jsonBuffer.createArray();
    const char* str = "hello";

    arr.add(str);
    _array.set(0, arr[0]);

    REQUIRE(str == _array[0]);
  }

  SECTION("array subscript add") {
    JsonArray& arr = _jsonBuffer.createArray();
    const char* str = "hello";

    arr.add(str);
    _array.add(arr[0]);

    REQUIRE(2 == _array.size());
    REQUIRE(9876 == _array[0]);
    REQUIRE(str == _array[1]);
  }

  SECTION("array subscript move assignment") {
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

  SECTION("object subscript set") {
    JsonObject& obj = _jsonBuffer.createObject();
    const char* str = "hello";

    obj["x"] = str;
    _array.set(0, obj["x"]);

    REQUIRE(str == _array[0]);
  }

  SECTION("object move-only subscript set") {
    JsonObject& obj = _jsonBuffer.createObject();
    const char* str = "hello";

    obj["x"] = str;
    _array.set(0, obj[std::string("x")]);

    REQUIRE(str == _array[0]);
  }

  SECTION("object subscript add") {
    JsonObject& obj = _jsonBuffer.createObject();
    const char* str = "hello";

    obj["x"] = str;
    _array.add(obj["x"]);

    REQUIRE(2 == _array.size());
    REQUIRE(9876 == _array[0]);
    REQUIRE(str == _array[1]);
  }

  SECTION("object move-only subscript add") {
    JsonObject& obj = _jsonBuffer.createObject();
    const char* str = "hello";

    obj["x"] = str;
    _array.add(obj[std::string("x")]);

    REQUIRE(2 == _array.size());
    REQUIRE(9876 == _array[0]);
    REQUIRE(str == _array[1]);
  }

  SECTION("object subscript move assignment") {
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
    const char buffer[] = "world";
    _array[0] = const_cast<char*>(&buffer[0]);
    REQUIRE(JSON_ARRAY_SIZE(1) == _jsonBuffer.size());
  }

  SECTION("should duplicate long char*") {
    const char buffer[] = "thisstringisaprettybigonetofitintothevariantstringbuffer";
    _array[0] = const_cast<char*>(&buffer[0]);
    REQUIRE((sizeof(buffer) + JSON_ARRAY_SIZE(1)) == _jsonBuffer.size());
  }

  SECTION("should use variant string buffer for short std::string") {
    _array[0] = std::string("world");
    REQUIRE(JSON_ARRAY_SIZE(1) == _jsonBuffer.size());
  }

  SECTION("should duplicate long std::string") {
    std::string val = "thisstringistoolongforthisarray";
    _array[0] = val;
    REQUIRE((val.length() + 1 + JSON_ARRAY_SIZE(1)) == _jsonBuffer.size());
  }
}
