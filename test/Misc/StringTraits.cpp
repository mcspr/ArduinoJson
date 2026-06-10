// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

#include <string>

using ArduinoJson::Internals::RawJsonString;
using ArduinoJson::Internals::StringTraits;
using ArduinoJson::Internals::ShouldDuplicate;

template <typename String>
constexpr bool should_duplicate() {
  return ShouldDuplicate<StringTraits<String>>::value;
}

TEST_CASE("StringTraits") {
  SECTION("should_duplicate") {
    STATIC_REQUIRE_FALSE(should_duplicate<const char*>());
    STATIC_REQUIRE(should_duplicate<char*>());
    STATIC_REQUIRE(should_duplicate<std::string>());
    STATIC_REQUIRE(should_duplicate<std::string&>());
    STATIC_REQUIRE(should_duplicate<const std::string&>());
    STATIC_REQUIRE(should_duplicate<RawJsonString<char*> >());
    STATIC_REQUIRE_FALSE(should_duplicate<RawJsonString<const char*> >());
    STATIC_REQUIRE(should_duplicate<RawJsonString<std::string> >());
    STATIC_REQUIRE(should_duplicate<RawJsonString<std::string&> >());
    STATIC_REQUIRE(should_duplicate<RawJsonString<const std::string&> >());
  }
}
