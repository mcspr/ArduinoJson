// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#define ARDUINOJSON_ENABLE_COMMENTS 1
#include <ArduinoJson.h>
#include <catch.hpp>
#include <sstream>

TEST_CASE("std::stream w/ Comments") {
  SECTION("Serialization") {
    SECTION("JsonVariantFalse") {
      std::ostringstream os;
      JsonVariant variant = false;
      os << variant;
      REQUIRE("false" == os.str());
    }

    SECTION("JsonVariantString") {
      std::ostringstream os;
      JsonVariant variant = "coucou";
      os << variant;
      REQUIRE("\"coucou\"" == os.str());
    }

    SECTION("JsonObject") {
      std::ostringstream os;
      DynamicJsonBuffer jsonBuffer;
      JsonObject& object = jsonBuffer.createObject();
      object["key"] = "value";
      os << object;
      REQUIRE("{\"key\":\"value\"}" == os.str());
    }

    SECTION("JsonObjectSubscript") {
      std::ostringstream os;
      DynamicJsonBuffer jsonBuffer;
      JsonObject& object = jsonBuffer.createObject();
      object["key"] = "value";
      os << object["key"];
      REQUIRE("\"value\"" == os.str());
    }

    SECTION("JsonArray") {
      std::ostringstream os;
      DynamicJsonBuffer jsonBuffer;
      JsonArray& array = jsonBuffer.createArray();
      array.add("value");
      os << array;
      REQUIRE("[\"value\"]" == os.str());
    }

    SECTION("JsonArraySubscript") {
      std::ostringstream os;
      DynamicJsonBuffer jsonBuffer;
      JsonArray& array = jsonBuffer.createArray();
      array.add("value");
      os << array[0];
      REQUIRE("\"value\"" == os.str());
    }
  }

  SECTION("Deserialization") {
    SECTION("ParseArray") {
      std::istringstream json(" [ \n 42\n ] ");
      DynamicJsonBuffer jsonBuffer;
      JsonArray& arr = jsonBuffer.parseArray(json);
      REQUIRE(arr.success());
      REQUIRE(1 == arr.size());
      REQUIRE(42 == arr[0]);
    }

    SECTION("ParseArray w/ Comments") {
      std::istringstream json("/* ignore */ [\n// these\n42 /* comments */ ]\x7f");
      DynamicJsonBuffer jsonBuffer;
      JsonArray& arr = jsonBuffer.parseArray(json);
      REQUIRE(arr.success());
      REQUIRE(1 == arr.size());
      REQUIRE(42 == arr[0]);
    }

    SECTION("ParseObject") {
      std::istringstream json(" { hello : world \n }");
      DynamicJsonBuffer jsonBuffer;
      JsonObject& obj = jsonBuffer.parseObject(json);
      REQUIRE(obj.success());
      REQUIRE(1 == obj.size());
      REQUIRE(std::string("world") == obj["hello"]);
    }

    SECTION("ParseObject w/ Comments") {
      std::istringstream json(" // another case\n{ hello : world // of line comments\n /* that are skipped when parsing */ }\x7f");
      DynamicJsonBuffer jsonBuffer;
      JsonObject& obj = jsonBuffer.parseObject(json);
      REQUIRE(obj.success());
      REQUIRE(1 == obj.size());
      REQUIRE(std::string("world") == obj["hello"]);
    }

    SECTION("ShouldNotReadPastTheEnd") {
      std::istringstream json("{}\x7f\x80\x81");
      DynamicJsonBuffer jsonBuffer;
      jsonBuffer.parseObject(json);
      REQUIRE('\x7f' == json.get());
    }

    SECTION("ShouldNotReadPastTheEnd w/ Comments") {
      std::istringstream json("/* hello world */\n{}/invalid comment");
      DynamicJsonBuffer jsonBuffer;
      jsonBuffer.parseObject(json);
      REQUIRE('/' == json.get());
    }
  }
}
