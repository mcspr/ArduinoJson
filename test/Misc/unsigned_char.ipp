// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

#ifndef ARDUINOJSON_UNSIGNED_CHAR_TEST_CASE
#define ARDUINOJSON_UNSIGNED_CHAR_TEST_CASE "signed / unsigned char string"
#endif

TEMPLATE_TEST_CASE(ARDUINOJSON_UNSIGNED_CHAR_TEST_CASE,
    "[template][char][unsigned char]",
    signed char, unsigned char)
{
  SECTION("JsonBuffer::parseArray") {
    TestType json[] = "[42]";

    StaticJsonBuffer<JSON_ARRAY_SIZE(1)> jsonBuffer;
    JsonArray& arr = jsonBuffer.parseArray(json);

    REQUIRE(arr.success());
  }

  SECTION("JsonBuffer::parseObject") {
    TestType json[] = "{\"a\":42}";

    StaticJsonBuffer<JSON_OBJECT_SIZE(1)> jsonBuffer;
    JsonObject& obj = jsonBuffer.parseObject(json);

    REQUIRE(obj.success());
  }

  SECTION("JsonVariant constructor") {
    TestType value[] = "42";

    JsonVariant variant(value);

    REQUIRE(42 == variant.as<int>());
  }

  SECTION("JsonVariant assignment operator") {
    TestType value[] = "42";

    JsonVariant variant(666);
    variant = value;

    REQUIRE(42 == variant.as<int>());
  }

  SECTION("JsonVariant::operator==") {
    TestType comparand[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    const JsonVariant variant = "hello";

    REQUIRE(comparand == variant);
    REQUIRE(variant == comparand);
    REQUIRE_FALSE(comparand != variant);
    REQUIRE_FALSE(variant != comparand);
  }

  SECTION("JsonVariant::operator!=") {
    TestType comparand[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    const JsonVariant variant = "world";

    REQUIRE(comparand != variant);
    REQUIRE(variant != comparand);
    REQUIRE_FALSE(comparand == variant);
    REQUIRE_FALSE(variant == comparand);
  }

  SECTION("JsonObject::operator[]") {
    TestType key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj[key] = "world";

    REQUIRE(std::string("world") == obj["hello"]);
  }

  SECTION("JsonObjectSubscript::operator=") {  // issue #416
    TestType value[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj["hello"] = value;

    REQUIRE(std::string("world") == obj["hello"]);
  }

  SECTION("JsonObjectSubscript::set()") {
    TestType value[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj["hello"].set(value);

    REQUIRE(std::string("world") == obj["hello"]);
  }

  SECTION("JsonObject::operator[] const") {
    TestType key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    const JsonObject& obj = jsonBuffer.parseObject("{\"hello\":\"world\"}");

    REQUIRE(std::string("world") == obj[key]);
  }

  SECTION("JsonObject::get()") {
    TestType key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.parseObject("{\"hello\":\"world\"}");

    REQUIRE(std::string("world") == obj.get<char*>(key));
  }

  SECTION("JsonObject::set() key") {
    TestType key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj.set(key, "world");

    REQUIRE(std::string("world") == obj["hello"]);
  }

  SECTION("JsonObject::set() value") {
    TestType value[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj.set("hello", value);

    REQUIRE(std::string("world") == obj["hello"]);
  }

  SECTION("JsonObject::set key&value") {
    TestType key[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj.set(key, key);

    REQUIRE(std::string("world") == obj["world"]);
  }

  SECTION("JsonObject::containsKey()") {
    TestType key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    const JsonObject& obj = jsonBuffer.parseObject("{\"hello\":\"world\"}");

    REQUIRE(true == obj.containsKey(key));
  }

  SECTION("JsonObject::remove()") {
    TestType key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.parseObject("{\"hello\":\"world\"}");
    obj.remove(key);

    REQUIRE(0 == obj.size());
  }

  SECTION("JsonObject::is()") {
    TestType key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.parseObject("{\"hello\":42}");

    REQUIRE(true == obj.is<int>(key));
  }

  SECTION("JsonObject::createNestedArray()") {
    TestType key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj.createNestedArray(key);
  }

  SECTION("JsonObject::createNestedObject()") {
    TestType key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj.createNestedObject(key);
  }

  SECTION("JsonArray::add()") {
    TestType value[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonArray& arr = jsonBuffer.createArray();
    arr.add(value);

    REQUIRE(std::string("world") == arr[0]);
  }

  SECTION("JsonArray::set()") {
    TestType value[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonArray& arr = jsonBuffer.createArray();
    arr.add("hello");
    arr.set(0, value);

    REQUIRE(std::string("world") == arr[0]);
  }

  SECTION("JsonArraySubscript::set()") {
    TestType value[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonArray& arr = jsonBuffer.createArray();
    arr.add("hello");
    arr[0].set(value);

    REQUIRE(std::string("world") == arr[0]);
  }

  SECTION("JsonArraySubscript::operator=") {
    TestType value[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonArray& arr = jsonBuffer.createArray();
    arr.add("hello");
    arr[0] = value;

    REQUIRE(std::string("world") == arr[0]);
  }
}
