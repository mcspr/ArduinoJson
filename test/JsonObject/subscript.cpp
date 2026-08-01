// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

static constexpr const char* const_char_nullptr = nullptr;

TEST_CASE("JsonObject::operator[]") {
  DynamicJsonBuffer _jsonBuffer;
  JsonObject& _object = _jsonBuffer.createObject();

  SECTION("char") {
    _object["hello"] = 1;

    REQUIRE(_object["hello"].is<char>());
    REQUIRE(1 == _object["hello"].as<char>());
    REQUIRE(1 == _object["hello"].as<short>());
    REQUIRE(1 == _object["hello"].as<int>());
    REQUIRE(1 == _object["hello"].as<long>());
    REQUIRE_FALSE(_object["hello"].is<const char*>());
    REQUIRE_FALSE(_object["hello"].is<bool>());
  }

  SECTION("short") {
    _object["hello"] = 1234;

    REQUIRE(_object["hello"].is<char>());
    REQUIRE(_object["hello"].is<short>());
    REQUIRE(_object["hello"].is<int>());
    REQUIRE(_object["hello"].is<long>());
    REQUIRE(0 == _object["hello"].as<char>());
    REQUIRE(1234 == _object["hello"].as<short>());
    REQUIRE(1234 == _object["hello"].as<int>());
    REQUIRE(1234 == _object["hello"].as<long>());
    REQUIRE_FALSE(_object["hello"].is<const char*>());
    REQUIRE_FALSE(_object["hello"].is<bool>());
  }

  SECTION("int") {
    _object["hello"] = 123456;

    REQUIRE(_object["hello"].is<char>());
    REQUIRE(_object["hello"].is<short>());
    REQUIRE(_object["hello"].is<int>());
    REQUIRE(_object["hello"].is<long>());
    REQUIRE(0 == _object["hello"].as<char>());
    REQUIRE(0 == _object["hello"].as<short>());
    REQUIRE(123456 == _object["hello"].as<int>());
    REQUIRE(123456 == _object["hello"].as<long>());
    REQUIRE_FALSE(_object["hello"].is<const char*>());
    REQUIRE_FALSE(_object["hello"].is<bool>());
  }

  SECTION("volatile int") {  // issue #415
    volatile int i = 123;
    _object["hello"] = i;

    REQUIRE(_object["hello"].is<int>());
    REQUIRE(123 == _object["hello"].as<int>());
    REQUIRE_FALSE(_object["hello"].is<bool>());
  }

  SECTION("double") {
    _object["hello"] = 123.45;

    REQUIRE(_object["hello"].is<double>());
    REQUIRE(Approx(123.45) == _object["hello"].as<double>());
    REQUIRE_FALSE(_object["hello"].is<char>());
    REQUIRE_FALSE(_object["hello"].is<short>());
    REQUIRE_FALSE(_object["hello"].is<int>());
    REQUIRE_FALSE(_object["hello"].is<long>());
  }

  SECTION("bool") {
    _object["hello"] = true;

    REQUIRE(_object["hello"].is<bool>());
    REQUIRE(_object["hello"].as<bool>());
    REQUIRE_FALSE(_object["hello"].is<char>());
    REQUIRE_FALSE(_object["hello"].is<short>());
    REQUIRE_FALSE(_object["hello"].is<int>());
    REQUIRE_FALSE(_object["hello"].is<long>());
  }

  SECTION("const char*") {
    _object["hello"] = "h3110";

    REQUIRE(_object["hello"].is<const char*>());
    REQUIRE_FALSE(_object["hello"].is<char>());
    REQUIRE_FALSE(_object["hello"].is<short>());
    REQUIRE_FALSE(_object["hello"].is<int>());
    REQUIRE_FALSE(_object["hello"].is<long>());
    REQUIRE(std::string("h3110") == _object["hello"].as<const char*>());
    REQUIRE(std::string("h3110") ==
            _object["hello"].as<char*>());  // <- short hand
  }

  SECTION("array") {
    JsonArray& arr = _jsonBuffer.createArray();

    _object["hello"] = arr;

    REQUIRE(std::addressof(arr) == std::addressof(_object["hello"].as<JsonArray&>()));
    REQUIRE(std::addressof(arr) == std::addressof(_object["hello"].as<JsonArray>()));  // <- short hand
    REQUIRE(std::addressof(arr) == std::addressof(_object["hello"].as<const JsonArray&>()));
    REQUIRE(std::addressof(arr) == std::addressof(_object["hello"].as<const JsonArray>()));  // <- short hand
    REQUIRE(_object["hello"].is<JsonArray&>());
    REQUIRE(_object["hello"].is<JsonArray>());
    REQUIRE(_object["hello"].is<const JsonArray&>());
    REQUIRE(_object["hello"].is<const JsonArray>());
    REQUIRE_FALSE(_object["hello"].is<JsonObject&>());
    REQUIRE_FALSE(_object["hello"].is<JsonObject>());
    REQUIRE_FALSE(_object["hello"].is<const JsonObject&>());
    REQUIRE_FALSE(_object["hello"].is<const JsonObject>());
  }

  SECTION("object") {
    JsonObject& obj = _jsonBuffer.createObject();

    _object["hello"] = obj;

    REQUIRE(std::addressof(obj) == std::addressof(_object["hello"].as<JsonObject&>()));
    REQUIRE(std::addressof(obj) == std::addressof(_object["hello"].as<JsonObject>()));  // <- short hand
    REQUIRE(std::addressof(obj) == std::addressof(_object["hello"].as<const JsonObject&>()));
    REQUIRE(std::addressof(obj) == std::addressof(_object["hello"].as<const JsonObject>()));  // <- short hand
    REQUIRE(_object["hello"].is<JsonObject&>());
    REQUIRE(_object["hello"].is<JsonObject>());
    REQUIRE(_object["hello"].is<const JsonObject&>());
    REQUIRE(_object["hello"].is<const JsonObject>());
    REQUIRE_FALSE(_object["hello"].is<JsonArray&>());
    REQUIRE_FALSE(_object["hello"].is<JsonArray>());
    REQUIRE_FALSE(_object["hello"].is<const JsonArray&>());
    REQUIRE_FALSE(_object["hello"].is<const JsonArray>());
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
