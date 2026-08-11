// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("const unsigned char string") {
  SECTION("JsonBuffer::parseArray") {
    const unsigned char json[] = "[42]";

    StaticJsonBuffer<JSON_ARRAY_SIZE(1) + 8> jsonBuffer;
    JsonArray& arr = jsonBuffer.parseArray(json);

    REQUIRE(arr.success());
  }

  SECTION("JsonBuffer::parseObject") {
    const unsigned char json[] = "{\"a\":42}";

    StaticJsonBuffer<JSON_OBJECT_SIZE(1) + 16> jsonBuffer;
    JsonObject& obj = jsonBuffer.parseObject(json);

    REQUIRE(obj.success());
  }

  SECTION("JsonVariant constructor") {
    const unsigned char value[] = "42";

    JsonVariant variant(value);

    REQUIRE(42 == variant.as<int>());
  }

  SECTION("JsonVariant assignment operator") {
    const unsigned char value[] = "42";

    JsonVariant variant(666);
    variant = value;

    REQUIRE(42 == variant.as<int>());
  }

  SECTION("JsonVariant::operator==") {
    const unsigned char comparand[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    const JsonVariant variant = "hello";

    REQUIRE(comparand == variant);
    REQUIRE(variant == comparand);
    REQUIRE_FALSE(comparand != variant);
    REQUIRE_FALSE(variant != comparand);
  }

  SECTION("JsonVariant::operator!=") {
    const unsigned char comparand[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    const JsonVariant variant = "world";

    REQUIRE(comparand != variant);
    REQUIRE(variant != comparand);
    REQUIRE_FALSE(comparand == variant);
    REQUIRE_FALSE(variant == comparand);
  }

  SECTION("JsonObject::operator[]") {
    const unsigned char key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj[key] = "world";

    REQUIRE(std::string("world") == obj["hello"]);
  }

  SECTION("JsonObjectSubscript::operator=") {  // issue #416
    const unsigned char value[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj["hello"] = value;

    REQUIRE(std::string("world") == obj["hello"]);
  }

  SECTION("JsonObjectSubscript::set()") {
    const unsigned char value[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj["hello"].set(value);

    REQUIRE(std::string("world") == obj["hello"]);
  }

  SECTION("JsonObject::operator[] const") {
    const unsigned char key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    const JsonObject& obj = jsonBuffer.parseObject("{\"hello\":\"world\"}");

    REQUIRE(std::string("world") == obj[key]);
  }

  SECTION("JsonObject::get()") {
    const unsigned char key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.parseObject("{\"hello\":\"world\"}");

    REQUIRE(std::string("world") == obj.get<char*>(key));
  }

  SECTION("JsonObject::set() key") {
    const unsigned char key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj.set(key, "world");

    REQUIRE(std::string("world") == obj["hello"]);
  }

  SECTION("JsonObject::set() value") {
    const unsigned char value[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj.set("hello", value);

    REQUIRE(std::string("world") == obj["hello"]);
  }

  SECTION("JsonObject::set key&value") {
    const unsigned char key[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj.set(key, key);

    REQUIRE(std::string("world") == obj["world"]);
  }

  SECTION("JsonObject::containsKey()") {
    const unsigned char key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    const JsonObject& obj = jsonBuffer.parseObject("{\"hello\":\"world\"}");

    REQUIRE(true == obj.containsKey(key));
  }

  SECTION("JsonObject::remove()") {
    const unsigned char key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.parseObject("{\"hello\":\"world\"}");
    obj.remove(key);

    REQUIRE(0 == obj.size());
  }

  SECTION("JsonObject::is()") {
    const unsigned char key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.parseObject("{\"hello\":42}");

    REQUIRE(true == obj.is<int>(key));
  }

  SECTION("JsonObject::createNestedArray()") {
    const unsigned char key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj.createNestedArray(key);
  }

  SECTION("JsonObject::createNestedObject()") {
    const unsigned char key[] = "hello";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();
    obj.createNestedObject(key);
  }

  SECTION("JsonArray::add()") {
    const unsigned char value[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonArray& arr = jsonBuffer.createArray();
    arr.add(value);

    REQUIRE(std::string("world") == arr[0]);
  }

  SECTION("JsonArray::set()") {
    const unsigned char value[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonArray& arr = jsonBuffer.createArray();
    arr.add("hello");
    arr.set(0, value);

    REQUIRE(std::string("world") == arr[0]);
  }

  SECTION("JsonArraySubscript::set()") {
    const unsigned char value[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonArray& arr = jsonBuffer.createArray();
    arr.add("hello");
    arr[0].set(value);

    REQUIRE(std::string("world") == arr[0]);
  }

  SECTION("JsonArraySubscript::operator=") {
    const unsigned char value[] = "world";

    DynamicJsonBuffer jsonBuffer;
    JsonArray& arr = jsonBuffer.createArray();
    arr.add("hello");
    arr[0] = value;

    REQUIRE(std::string("world") == arr[0]);
  }
}
