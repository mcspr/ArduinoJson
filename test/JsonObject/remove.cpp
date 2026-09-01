// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>
#include <string>

TEST_CASE("JsonObject::remove()") {
  DynamicJsonBuffer jb;

  SECTION("SizeDecreased_WhenValuesAreRemoved") {
    JsonObject& obj = jb.createObject();
    obj["hello"] = 1;

    REQUIRE(obj.remove("hello") == obj.end());
    REQUIRE(0 == obj.size());
  }

  SECTION("SizeUntouched_WhenRemoveIsCalledWithAWrongKey") {
    JsonObject& obj = jb.createObject();
    obj["hello"] = 1;

    REQUIRE(obj.remove("world") == obj.end());
    REQUIRE(1 == obj.size());
  }

  SECTION("RemoveByIterator") {
    JsonObject& obj = jb.parseObject("{\"a\":0,\"b\":1,\"c\":2,\"d\":3}");
    REQUIRE(4 == obj.size());

    for (auto it = obj.begin(); it != obj.end(); ++it) {
      if (it->value == 1) {
        it = obj.remove(it);
        it = obj.remove(it);
      }
    }

    REQUIRE(2 == obj.size());

    REQUIRE_FALSE(obj["b"].success());
    REQUIRE_FALSE(obj["c"].success());

    REQUIRE(obj["a"].success());
    REQUIRE(obj["a"] == 0);

    REQUIRE(obj["d"].success());
    REQUIRE(obj["d"] == 3);
  }
}
