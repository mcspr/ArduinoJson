// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonArray::add()") {
  DynamicJsonBuffer _jsonBuffer;
  JsonArray& _array = _jsonBuffer.createArray();

  SECTION("int") {
    _array.add(123);
    REQUIRE(123 == _array[0].as<int>());
    REQUIRE(_array[0].is<int>());
    REQUIRE(_array[0].is<double>());
  }

  SECTION("double") {
    _array.add(123.45);
    REQUIRE(123.45 == _array[0].as<double>());
    REQUIRE(_array[0].is<double>());
    REQUIRE_FALSE(_array[0].is<bool>());
  }

  SECTION("bool") {
    _array.add(true);
    REQUIRE(true == _array[0].as<bool>());
    REQUIRE(_array[0].is<bool>());
    REQUIRE_FALSE(_array[0].is<int>());
  }

  SECTION("const char*") {
    const char* str = "hello";
    _array.add(str);
    REQUIRE(str == _array[0].as<std::string>());
    REQUIRE(_array[0].is<const char*>());
    REQUIRE_FALSE(_array[0].is<int>());
  }

  SECTION("nested array") {
    JsonArray& arr = _jsonBuffer.createArray();

    _array.add(arr);

    REQUIRE(&arr == &_array[0].as<JsonArray&>());
    REQUIRE(_array[0].is<JsonArray&>());
    REQUIRE_FALSE(_array[0].is<int>());
  }

  SECTION("nested object") {
    JsonObject& obj = _jsonBuffer.createObject();

    _array.add(obj);

    REQUIRE(&obj == &_array[0].as<JsonObject&>());
    REQUIRE(_array[0].is<JsonObject&>());
    REQUIRE_FALSE(_array[0].is<int>());
  }

  SECTION("array subscript") {
    const char* str = "hello";
    JsonArray& arr = _jsonBuffer.createArray();
    arr.add(str);

    _array.add(arr[0]);

    REQUIRE(str == _array[0]);
  }

  SECTION("object subscript") {
    const char* str = "hello";
    JsonObject& obj = _jsonBuffer.createObject();
    obj["x"] = str;

    _array.add(obj["x"]);

    REQUIRE(str == _array[0]);
  }

  SECTION("should not duplicate const char*") {
    const char* data = "world";
    _array.add(data);
    REQUIRE(_jsonBuffer.size() == JSON_ARRAY_SIZE(1));
    REQUIRE(_array.size() == 1);
    REQUIRE(_array[0].as<const char*>() == data);
  }

  SECTION("should use variant string buffer for short char*") {
    const char* data = "world";
    _array.add(const_cast<char*>(data));
    REQUIRE(_jsonBuffer.size() == JSON_ARRAY_SIZE(1));
    REQUIRE(_array.size() == 1);
    REQUIRE(_array[0].as<const char*>() != data);
  }

  SECTION("should duplicate long char*") {
    const char* data = "allocatethisstringontheheapinsteadofthevariantstringbuffer";
    _array.add(const_cast<char*>(data));
    REQUIRE(_jsonBuffer.size() > JSON_ARRAY_SIZE(1));
    REQUIRE(_array.size() == 1);
    REQUIRE(_array[0].as<const char*>() != nullptr);
    REQUIRE(_array[0].as<const char*>() != data);
    REQUIRE(_array[0].as<const char*>() == std::string(data));
  }

  SECTION("should use variant string buffer for short std::string") {
    std::string data("world");
    _array.add(data);
    REQUIRE(_jsonBuffer.size() == JSON_ARRAY_SIZE(1));
    REQUIRE(_array.size() == 1);
    REQUIRE(_array[0].as<const char*>() != data.c_str());
  }

  SECTION("should duplicate long std::string") {
    std::string data("thisstringwouldbeallocatedonheapandnotbeinplace");
    _array.add(data);
    REQUIRE(_jsonBuffer.size() > JSON_ARRAY_SIZE(1));
    REQUIRE(_array.size() == 1);
    REQUIRE(_array[0].as<const char*>() != nullptr);
    REQUIRE(_array[0].as<const char*>() != data.c_str());
    REQUIRE(_array[0].as<const char*>() == data);
  }

  SECTION("should not duplicate RawJson(const char*)") {
    const char* data = "{}";
    _array.add(RawJson(data));
    REQUIRE(_jsonBuffer.size() == JSON_ARRAY_SIZE(1));
    REQUIRE(_array.size() == 1);
    REQUIRE(_array[0].as<const char*>() == data);
  }

  SECTION("should use variant string buffer for short RawJson(char*)") {
    const char* data = "{}";
    _array.add(RawJson(const_cast<char*>(data)));
    REQUIRE(_jsonBuffer.size() == JSON_ARRAY_SIZE(1));
    REQUIRE(_array.size() == 1);
    REQUIRE(_array[0].as<const char*>() != data);
  }

  SECTION("should duplicate long RawJson(char*)") {
    const char* data = "{hello1: world1, hello2: world2, hello3: world3, hello4: world4}";
    _array.add(RawJson(const_cast<char*>(data)));
    REQUIRE(_jsonBuffer.size() > JSON_ARRAY_SIZE(1));
    REQUIRE(_array.size() == 1);
    REQUIRE(_array[0].as<const char*>() != data);
  }

  SECTION("should use variant string buffer for short RawJson(std::string)") {
    _array.add(RawJson(std::string("{}")));
    REQUIRE(_jsonBuffer.size() == JSON_ARRAY_SIZE(1));
    REQUIRE(_array.size() == 1);
    REQUIRE(_array[0].as<const char*>() != nullptr);
    REQUIRE(_array[0].as<const char*>() == std::string("{}"));
  }

  SECTION("should duplicate long RawJson(std::string)") {
    std::string data = "{hello1: world1, hello2: world2, hello3: world3, hello4: world4}";
    _array.add(RawJson(data));
    REQUIRE(_jsonBuffer.size() > JSON_ARRAY_SIZE(1));
    REQUIRE(_array.size() == 1);
    REQUIRE(_array[0].as<const char*>() != data.c_str());
    REQUIRE(_array[0].as<const char*>() == data);
  }
}
