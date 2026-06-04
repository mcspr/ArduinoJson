// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

#include <string>

using ArduinoJson::Internals::RawJsonString;

template <typename String>
constexpr bool should_duplicate() {
  return ArduinoJson::Internals::StringTraits<String>::should_duplicate::value;
}

TEST_CASE("StringTraits") {
  SECTION("should_duplicate") {
    REQUIRE(false == should_duplicate<const char*>());
    REQUIRE(true == should_duplicate<char*>());
    REQUIRE(true == should_duplicate<std::string>());
    REQUIRE(true == should_duplicate<std::string&>());
    REQUIRE(true == should_duplicate<const std::string&>());
    REQUIRE(true == should_duplicate<RawJsonString<char*> >());
    REQUIRE(false == should_duplicate<RawJsonString<const char*> >());
    REQUIRE(true == should_duplicate<RawJsonString<std::string> >());
    REQUIRE(true == should_duplicate<RawJsonString<std::string&> >());
    REQUIRE(true == should_duplicate<RawJsonString<const std::string&> >());
  }
}
