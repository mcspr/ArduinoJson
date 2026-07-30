// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

static constexpr const char* const_char_nullptr = nullptr;

TEST_CASE("JsonObject::operator[]") {
  DynamicJsonBuffer _jsonBuffer;
  JsonObject& _object = _jsonBuffer.createObject();

  SECTION("int") {
    _object["hello"] = 123;

    REQUIRE(123 == _object["hello"].as<int>());
    REQUIRE(true == _object["hello"].is<int>());
    REQUIRE(false == _object["hello"].is<bool>());
  }

  SECTION("volatile int") {  // issue #415
    volatile int i = 123;
    _object["hello"] = i;

    REQUIRE(123 == _object["hello"].as<int>());
    REQUIRE(true == _object["hello"].is<int>());
    REQUIRE(false == _object["hello"].is<bool>());
  }

  SECTION("double") {
    _object["hello"] = 123.45;

    REQUIRE(true == _object["hello"].is<double>());
    REQUIRE(false == _object["hello"].is<long>());
    REQUIRE(123.45 == _object["hello"].as<double>());
  }

  SECTION("bool") {
    _object["hello"] = true;

    REQUIRE(true == _object["hello"].is<bool>());
    REQUIRE(false == _object["hello"].is<long>());
    REQUIRE(true == _object["hello"].as<bool>());
  }

  SECTION("const char*") {
    _object["hello"] = "h3110";

    REQUIRE(true == _object["hello"].is<const char*>());
    REQUIRE(false == _object["hello"].is<long>());
    REQUIRE(std::string("h3110") == _object["hello"].as<const char*>());
    REQUIRE(std::string("h3110") ==
            _object["hello"].as<char*>());  // <- short hand
  }

  SECTION("array") {
    JsonArray& arr = _jsonBuffer.createArray();

    _object["hello"] = arr;

    REQUIRE(&arr == &_object["hello"].as<JsonArray&>());
    REQUIRE(&arr == &_object["hello"].as<JsonArray>());  // <- short hand
    REQUIRE(&arr == &_object["hello"].as<const JsonArray&>());
    REQUIRE(&arr == &_object["hello"].as<const JsonArray>());  // <- short hand
    REQUIRE(true == _object["hello"].is<JsonArray&>());
    REQUIRE(true == _object["hello"].is<JsonArray>());
    REQUIRE(true == _object["hello"].is<const JsonArray&>());
    REQUIRE(true == _object["hello"].is<const JsonArray>());
    REQUIRE(false == _object["hello"].is<JsonObject&>());
  }

  SECTION("object") {
    JsonObject& obj = _jsonBuffer.createObject();

    _object["hello"] = obj;

    REQUIRE(&obj == &_object["hello"].as<JsonObject&>());
    REQUIRE(&obj == &_object["hello"].as<JsonObject>());  // <- short hand
    REQUIRE(&obj == &_object["hello"].as<const JsonObject&>());
    REQUIRE(&obj == &_object["hello"].as<const JsonObject>());  // <- short hand
    REQUIRE(true == _object["hello"].is<JsonObject&>());
    REQUIRE(true == _object["hello"].is<JsonObject>());
    REQUIRE(true == _object["hello"].is<const JsonObject&>());
    REQUIRE(true == _object["hello"].is<const JsonObject>());
    REQUIRE(false == _object["hello"].is<JsonArray&>());
  }

  SECTION("array subscript") {
    JsonArray& arr = _jsonBuffer.createArray();
    arr.add(42);

    _object["a"] = arr[0];

    REQUIRE(42 == _object["a"]);
  }

  SECTION("object subscript") {
    JsonObject& obj = _jsonBuffer.createObject();
    obj.set("x", 42);

    _object["a"] = obj["x"];

    REQUIRE(42 == _object["a"]);
  }

  SECTION("char key[]") {  // issue #423
    char key[] = "hello";
    _object[key] = 42;
    REQUIRE(42 == _object[key]);
  }

  SECTION("should not duplicate const char[]") {
    _object["hello"] = "world";
    REQUIRE(JSON_OBJECT_SIZE(1) == _jsonBuffer.size());
  }

  SECTION("should not duplicate const char*") {
    const char* key = "hello";
    const char* val = "world";
    _object[key] = val;
    REQUIRE(JSON_OBJECT_SIZE(1) == _jsonBuffer.size());
  }

  SECTION("should duplicate char* value") {
    _object["hello"] = const_cast<char*>("longstringthatwouldallocate");
    REQUIRE(_jsonBuffer.size() > JSON_OBJECT_SIZE(1));
  }

  SECTION("should not duplicate small char* key") {
    _object[const_cast<char*>("hello")] = "world";
    REQUIRE(_jsonBuffer.size() == JSON_OBJECT_SIZE(1));
  }

  SECTION("should duplicate char* key") {
    _object[const_cast<char*>("thiskeyshouldgetduplicatedinthebuffer")] = "world";
    REQUIRE(_jsonBuffer.size() > JSON_OBJECT_SIZE(1));
  }

  SECTION("should not duplicate small char* key&value") {
    _object[const_cast<char*>("hello")] = const_cast<char*>("world");
    REQUIRE(_jsonBuffer.size() == JSON_OBJECT_SIZE(1));
  }

  SECTION("should duplicate long char* key&value") {
    _object[const_cast<char*>("thiskeyshouldgetallocatedinthebuffer")] =
      const_cast<char*>("thisvalshouldalsogetallocatedinthebuffer");
    REQUIRE(_jsonBuffer.size() > JSON_OBJECT_SIZE(1));
  }

  SECTION("should not duplicate small char[] key&value") {
    char key[] = "hello";
    char val[] = "world";
    _object[key] = val;
    REQUIRE(_jsonBuffer.size() == JSON_OBJECT_SIZE(1));
  }

  SECTION("should duplicate long char[] key&value") {
    char key[] = "thiskeyshouldgetallocated";
    char val[] = "thisvalshouldgetallocated";
    _object[key] = val;
    REQUIRE(_jsonBuffer.size() > JSON_OBJECT_SIZE(1));
  }

  SECTION("should duplicate std::string value") {
    _object["hello"] = std::string("longstringthatwouldallocate");
    REQUIRE(_jsonBuffer.size() > JSON_OBJECT_SIZE(1));
  }

  SECTION("should not duplicate small std::string key") {
    _object[std::string("hello")] = "world";
    REQUIRE(_jsonBuffer.size() == JSON_OBJECT_SIZE(1));
  }

  SECTION("should duplicate long std::string key") {
    _object[std::string("makingsurethatthisstringgetsallocated")] = "world";
    REQUIRE(_jsonBuffer.size() > JSON_OBJECT_SIZE(1));
  }

  SECTION("should not duplicate small std::string key&value") {
    _object[std::string("hello")] = std::string("world");
    REQUIRE(_jsonBuffer.size() == JSON_OBJECT_SIZE(1));
  }

  SECTION("should duplicate long std::string key&value") {
    _object[std::string("somekeyvaluethatislongerthaninternalbuffer")] =
      std::string("andsomevaluethatislongerthaninternalbuffer");
    REQUIRE(_jsonBuffer.size() > JSON_OBJECT_SIZE(1));
  }

  SECTION("should not duplicate small const std::string& key&value") {
    const auto key = std::string("hello");
    const auto val = std::string("world");
    _object[key] = val;
    REQUIRE(_jsonBuffer.size() == JSON_OBJECT_SIZE(1));
  }

  SECTION("should duplicate long const std::string& key&value") {
    const auto key = std::string("longstringthatwouldallocate");
    const auto val = std::string("anotherlongstringforvalue");
    _object[key] = val;
    REQUIRE(_jsonBuffer.size() > JSON_OBJECT_SIZE(1));
  }

  SECTION("should ignore null key") {
    // object must have a value to make a call to strcmp()
    _object["dummy"] = 42;
    _object[const_char_nullptr] = 666;

    REQUIRE(_object.size() == 1);
    REQUIRE_FALSE(_object[const_char_nullptr].success());
  }

  // impl detail. while subscript references the object, key is a separate entity and has its own lifetime

  SECTION("std::string& key not duplicated in subscript") {
    std::string key = "dummy";

    auto subscript = _object[key];
    subscript = "12345";

    REQUIRE(_object.size() == 1);
    REQUIRE(subscript.success());
    REQUIRE(_object["dummy"] == std::string("12345"));
    REQUIRE(subscript.as<const char*>() == std::string("12345"));

    key = "ymmud";

    REQUIRE(_object.size() == 1);
    REQUIRE(_object["dummy"] == std::string("12345"));
    REQUIRE_FALSE(subscript.success());
  }

  SECTION("std::string duplicated by value") {
    std::string val = "12345";

    auto subscript = _object["dummy"];
    subscript = val;

    REQUIRE(_object.size() == 1);
    REQUIRE(subscript.as<const char*>() == std::string("12345"));

    val = "67890";

    REQUIRE(_object.size() == 1);
    REQUIRE(subscript.as<const char*>() == std::string("12345"));
  }

  SECTION("std::string key constructed inplace") {
    auto subscript = _object[std::string("dummy")];
    subscript = "23456";

    REQUIRE(_object.size() == 1);
    REQUIRE(_object["dummy"] == std::string("23456"));
  }
}
