// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>
#include <string>
#include "ArduinoJson/Data/StringView.hpp"

TEST_CASE("JsonObject::set()") {
  DynamicJsonBuffer jb;
  JsonObject& _object = jb.createObject();

  SECTION("int") {
    _object.set("hello", 123);

    REQUIRE(123 == _object["hello"].as<int>());
    REQUIRE(_object["hello"].is<int>());
    REQUIRE_FALSE(_object["hello"].is<bool>());
  }

  SECTION("double") {
    _object.set("hello", 123.45);

    REQUIRE(123.45 == _object["hello"].as<double>());
    REQUIRE(_object["hello"].is<double>());
    REQUIRE_FALSE(_object["hello"].is<bool>());
  }

  SECTION("bool") {
    _object.set("hello", true);

    REQUIRE(_object["hello"].as<bool>());
    REQUIRE(_object["hello"].is<bool>());
    REQUIRE_FALSE(_object["hello"].is<long>());
  }

  SECTION("const char*") {
    _object.set("hello", "h3110");

    REQUIRE(std::string("h3110") == _object["hello"].as<const char*>());
    REQUIRE(_object["hello"].is<const char*>());
    REQUIRE_FALSE(_object["hello"].is<long>());
  }

  SECTION("nested array") {
    JsonArray& arr = jb.createArray();

    _object.set("hello", arr);

    REQUIRE(&arr == &_object["hello"].as<JsonArray>());
    REQUIRE(_object["hello"].is<JsonArray&>());
    REQUIRE_FALSE(_object["hello"].is<JsonObject&>());
  }

  SECTION("nested object") {
    JsonObject& obj = jb.createObject();

    _object.set("hello", obj);

    REQUIRE(&obj == &_object["hello"].as<JsonObject>());
    REQUIRE(_object["hello"].is<JsonObject&>());
    REQUIRE_FALSE(_object["hello"].is<JsonArray&>());
  }

  SECTION("array subscript") {
    JsonArray& arr = jb.createArray();
    arr.add(42);

    _object.set("a", arr[0]);

    REQUIRE(42 == _object["a"]);
  }

  SECTION("object subscript") {
    JsonObject& obj = jb.createObject();
    obj.set("x", 42);

    _object.set("a", obj["x"]);

    REQUIRE(42 == _object["a"]);
  }

  SECTION("returns true when allocation succeeds") {
    StaticJsonBuffer<JSON_OBJECT_SIZE(1) + 15> jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();

    REQUIRE(true == obj.set(std::string("hello"), std::string("world")));
  }

  SECTION("returns false when allocation fails") {
    StaticJsonBuffer<JSON_OBJECT_SIZE(1) + 10> jsonBuffer;
    JsonObject& obj = jsonBuffer.createObject();

    REQUIRE(false == obj.set(std::string("hello"), std::string("world")));
  }

  SECTION("should not duplicate const char*") {
    _object.set("hello", "world");
    REQUIRE(JSON_OBJECT_SIZE(1) == jb.size());
  }

  SECTION("should duplicate char* value") {
    const char* key = "hello";
    const char* val = "world";
    _object.set(key, const_cast<char*>(val));
    REQUIRE(jb.size() > JSON_OBJECT_SIZE(1));
    REQUIRE(_object.size() == 1);
    auto first = _object.begin();
    REQUIRE(first->key == key);
    REQUIRE(first->key == std::string(key));
    REQUIRE(first->value.as<const char*>() != nullptr);
    REQUIRE(first->value.as<const char*>() != val);
    REQUIRE(first->value == std::string(val));
  }

  SECTION("should duplicate char* key") {
    const char* key = "hello";
    _object.set(const_cast<char*>(key), "world");
    REQUIRE(jb.size() > JSON_OBJECT_SIZE(1));
    REQUIRE(_object.size() == 1);
    auto first = _object.begin();
    REQUIRE(first->key != key);
    REQUIRE(first->key == std::string(key));
    REQUIRE(first->value == std::string("world"));
  }

  SECTION("should duplicate char* key&value") {
    const char* key = "hello";
    const char* val = "world";
    _object.set(const_cast<char*>(key), const_cast<char*>(val));
    REQUIRE(jb.size() > JSON_OBJECT_SIZE(1));
    REQUIRE(_object.size() == 1);
    auto first = _object.begin();
    REQUIRE(first->key != key);
    REQUIRE(first->key == std::string(key));
    REQUIRE(first->value.as<const char*>() != nullptr);
    REQUIRE(first->value.as<const char*>() != val);
    REQUIRE(first->value == std::string(val));
  }

  SECTION("mutable view would duplicate part of the value") {
    char val[] = "helloworld";
    _object.set("hello", ArduinoJson::MakeStringView(&val[0], 5));
    REQUIRE(jb.size() > JSON_OBJECT_SIZE(1));
    auto first = _object.begin();
    REQUIRE(first->key == std::string("hello"));
    REQUIRE(first->value == std::string("hello"));
  }

  SECTION("immutable view would reference pointer as-is") {
    // TODO cstring pointer unaffected by size
    const char val[] = "helloworld";
    _object.set("hello", ArduinoJson::MakeStringView(&val[5], 1));
    REQUIRE(jb.size() == JSON_OBJECT_SIZE(1));
    auto first = _object.begin();
    REQUIRE(first->key == std::string("hello"));
    REQUIRE(first->value == std::string("world"));
  }

  SECTION("should duplicate std::string value") {
    _object.set("hello", std::string("world"));
    REQUIRE(jb.size() > JSON_OBJECT_SIZE(1));
  }

  SECTION("should duplicate std::string key") {
    _object.set(std::string("hello"), "world");
    REQUIRE(jb.size() > JSON_OBJECT_SIZE(1));
  }

  SECTION("should duplicate std::string key&value") {
    _object.set(std::string("hello"), std::string("world"));
    REQUIRE(jb.size() > JSON_OBJECT_SIZE(1));
  }
}
