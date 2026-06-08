// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson/Serialization/StaticStringBuilder.hpp>
#include <ArduinoJson/Serialization/DynamicStringBuilder.hpp>

#include <catch.hpp>

#include <algorithm>

// while the static version consumes and returns as 'written' the same amount of bytes as the dynamic,
// actual output is size + 1 b/c after every char or string write there is an additional '\0' written
using ArduinoJson::Internals::StaticStringBuilder;
using ArduinoJson::Internals::DynamicStringBuilder;

template <typename StringBuilder, typename String>
void common_tests(StringBuilder& sb, const String& output) {
  SECTION("InitialState") {
    REQUIRE(std::string("") == output);
  }

  SECTION("EmptyString") {
    REQUIRE(0 == sb.print(""));
    REQUIRE(std::string("") == output);
  }

  SECTION("OneString") {
    REQUIRE(4 == sb.print("ABCD"));
    REQUIRE(std::string("ABCD") == output);
  }

  SECTION("TwoStrings") {
    REQUIRE(4 == sb.print("ABCD"));
    REQUIRE(4 == sb.print("EFGH"));
    REQUIRE(std::string("ABCDEFGH") == output);
  }
}

TEST_CASE("StaticStringBuilder") {
  char output[20];
  std::fill(&output[0], &output[sizeof(output) - 1], '*');
  output[0] = '\0';

  StaticStringBuilder sb(output, sizeof(output));

  common_tests(sb, static_cast<const char*>(output));

  SECTION("OverCapacity") {
    REQUIRE((sizeof(output) - 1) == sb.print("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
    REQUIRE(0 == sb.print("ABC"));
    REQUIRE(std::string("ABCDEFGHIJKLMNOPQRS") == output);
  }
}

TEST_CASE("DynamicStringBuilder") {
  std::string output;
  DynamicStringBuilder<std::string> sb(output);
  common_tests(sb, output);
}
