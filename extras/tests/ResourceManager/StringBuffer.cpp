// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#include <ArduinoJson.hpp>
#include <catch.hpp>

#include "Allocators.hpp"
#include "Literals.hpp"

using namespace ArduinoJson::detail;

TEST_CASE("StringBuffer") {
  SpyingAllocator spy;
  ResourceManager resources(&spy);
  StringBuffer sb(&resources);
  VariantData variant;

  SECTION("Tiny string") {
    auto ptr = sb.reserve(3);
    strcpy(ptr, "hi!");
    sb.save(&variant);

    REQUIRE(variant.type == VariantType::TinyString);
    REQUIRE(VariantImpl(&variant, &resources).asString() == "hi!");
  }

  SECTION("Tiny string can't contain NUL") {
    auto ptr = sb.reserve(3);
    memcpy(ptr, "a\0b", 3);
    sb.save(&variant);

    REQUIRE(variant.type == VariantType::OwnedString);

    auto str = VariantImpl(&variant, &resources).asString();
    REQUIRE(str.size() == 3);
    REQUIRE(str.c_str()[0] == 'a');
    REQUIRE(str.c_str()[1] == 0);
    REQUIRE(str.c_str()[2] == 'b');
  }

  SECTION("Tiny string can't have 4 characters") {
    auto ptr = sb.reserve(4);
    strcpy(ptr, "alfa");
    sb.save(&variant);

    REQUIRE(variant.type == VariantType::OwnedString);
    REQUIRE(VariantImpl(&variant, &resources).asString() == "alfa");
  }
}
