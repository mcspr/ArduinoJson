// MIT License

#define ARDUINOJSON_VALIDATE_UTF8 0
#include <ArduinoJson.h>

#include <catch.hpp>

#include "escape.ipp"

TEST_CASE("JsonBuffer::parse() w/ invalid character sequences") {
  DynamicJsonBuffer jb;
  std::vector<std::string> testCases;

  auto shouldSucceed = [&]() {
    size_t n = 1;
    for (const auto& testCase : testCases) {
      CAPTURE(n);
      CAPTURE(escape(testCase));
      ++n;

      JsonVariant variant = jb.parse(testCase);
      REQUIRE(variant.success());
      REQUIRE(variant.is<const char*>());
    }
  };

  SECTION("Invalid UTF8 sequences") {
    testCases = {
      "'\xA0\xA1'",
      "'\\uA0A1'",
      "'\xC0\x9F'",
      "'\\uC09F'",
      "'\xE2\x28\xA1'",
      "'\\uE282\x28'",
      "'\xE2\\u8228'",
      "'\xE2\x82\x28'",
      "'\\uE282\x28'",
      "'\xE2\\u8228'",
      "'\xED\xA0\x80'",
      "'\\uEDA0\x80'",
      "'\xED\\uA080'",
      "'\xED\xA0\x81'",
      "'\\uEDA0\x81'",
      "'\xED\\uA081'",
      "'\xF0\x28\x8C\x28'",
      "'\\uF028\\u8C28'",
      "'\xF0\x28\x8C\xBC'",
      "'\\uF028\\u8CBC'",
      "'\xF0\x90\x28\xBC'",
      "'\\uF090\\u28BC'",
      "'\xF5\xFF\xFF\xFF'",
      "'\\uF5FF\\uFFFF'",
      "'\xF8\x90\x80\x80\x80'",
      "'\xF8\\u9080\\u8080'",
      "'\xC3\x28'",
      "'\\uC328'",
      "'\xc2\x7f'",
      "'\\uc27f'",
      "'\x80'",
      "'\\u0080'",
      "'\x90'",
      "'\\uc290'",
      "'\xa1'",
      "'\\u00a1'",
      "'\xb2'",
      "'\\u00b2'",
      "'\xc3'",
      "'\\uc383'",
      "'\xd4'",
      "'\\uc394'",
      "'\xdf'",
      "'\\uc394'",
      "'\xe5'",
      "'\\uc3a5'",
      "'\xef\xbf'",
      "'\xef\xbf\xbf'",
      "'\\uefbf'",
      "'\xf6\xc3\xb1'",
      "'\xf6\\uc3b1'",
      "'\\uf6c3\xb1'",
      "'\xf4\x90\x80\x80'",
      "'\xf4\\u9080\x80'",
      "'\\uf490\\u8080'",
      "'\xef\xbb\xbf\x48\65\x6c\x6c\x6f'",
      "'\x7f\x4c\x23\x3c\x3a\x6f\x5d\x44\x23\x70'",
    };

    shouldSucceed();
  }
}
