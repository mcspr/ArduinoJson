// MIT License

#define ARDUINOJSON_ENABLE_UTF8_BOM 0
#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonBuffer::parse() w/ BOM parsing disabled") {
  DynamicJsonBuffer jb;

  SECTION("Empty string") {
    JsonVariant variant = jb.parse("");
    REQUIRE_FALSE(variant.success());
  }

  SECTION("Just BOM") {
    JsonVariant variant = jb.parse("\xef\xbb\xbf");
    REQUIRE_FALSE(variant.success());
  }

  SECTION("EmptyObject") {
    JsonVariant variant = jb.parse("\xef\xbb\xbf" "{}");
    REQUIRE_FALSE(variant.success());
  }

  SECTION("EmptyArray") {
    JsonVariant variant = jb.parse("\xef\xbb\xbf" "[]");
    REQUIRE_FALSE(variant.success());
  }

  SECTION("Null") {
    JsonVariant variant = jb.parse("\xef\xbb\xbf" "null");
    REQUIRE_FALSE(variant.success());
  }

  SECTION("False") {
    JsonVariant variant = jb.parse("\xef\xbb\xbf" "false");
    REQUIRE_FALSE(variant.success());
  }

  SECTION("True") {
    JsonVariant variant = jb.parse("\xef\xbb\xbf" "true");
    REQUIRE_FALSE(variant.success());
  }

  SECTION("Integer") {
    JsonVariant variant = jb.parse("\xef\xbb\xbf" "-42");
    REQUIRE_FALSE(variant.success());
  }

  SECTION("Double") {
    JsonVariant variant = jb.parse("\xef\xbb\xbf" "-1.23e+4");
    REQUIRE_FALSE(variant.success());
  }

  SECTION("Double quoted string") {
    std::string testCase = "\xef\xbb\xbf" "\"12345\"";
    JsonVariant variant = jb.parse(testCase);
    REQUIRE_FALSE(variant.success());
  }


  SECTION("Single quoted string") {
    std::string testCase = "\xef\xbb\xbf" "'12345'";
    JsonVariant variant = jb.parse(testCase);
    REQUIRE_FALSE(variant.success());
  }
}
