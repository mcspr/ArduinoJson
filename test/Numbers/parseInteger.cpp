// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson/Numbers/parseNumber.hpp>
#include <catch.hpp>

#include <cstdint>

using ArduinoJson::Internals::JsonNumberParser;

// 5.x branch does usually doesn't check for conversion validity and numbers would fall back to 0
// Numbers/parseNumber.hpp is backported from v7, allowing to perform some extra sanity checks

template <typename T, size_t Size>
void shouldParseAndConvert(const char (&input)[Size], T expected) {
  CAPTURE(input);
  const auto parse = JsonNumberParser::parse(&input[0], Size - 1);
  REQUIRE(parse);
  const auto convert = parse.template convertTo<T>();
  REQUIRE(convert);
  REQUIRE(convert.value == expected);
}

template <typename T, size_t Size>
void shouldNotParse(const char (&input)[Size]) {
  CAPTURE(input);
  const auto parse = JsonNumberParser::parse(&input[0], Size - 1);
  REQUIRE_FALSE(parse);
}

template <typename T, size_t Size>
void shouldNotConvert(const char (&input)[Size]) {
  CAPTURE(input);
  const auto parse = JsonNumberParser::parse(&input[0], Size - 1);
  REQUIRE(parse);
  const auto convert = parse.template convertTo<T>();
  REQUIRE_FALSE(convert);
}

TEST_CASE("parseInteger<int8_t>()") {
  shouldNotParse<int8_t>("");
  shouldParseAndConvert<int8_t>("0", 0);
  shouldParseAndConvert<int8_t>("-128", int8_t{-128});
  shouldParseAndConvert<int8_t>("127", int8_t{127});
  shouldParseAndConvert<int8_t>("+127", int8_t{127});
  shouldParseAndConvert<int8_t>("3.14", 3);
  shouldNotParse<int8_t>("true");
  shouldNotParse<int8_t>("false");
  shouldNotParse<int8_t>("x42");
  shouldNotParse<int8_t>("42x");
  shouldNotParse<int8_t>("1x5");
  shouldNotConvert<int8_t>("128");
  shouldNotConvert<int8_t>("-129");
}

TEST_CASE("parseInteger<int16_t>()") {
  shouldNotParse<int16_t>("");
  shouldParseAndConvert<int16_t>("0", 0);
  shouldParseAndConvert<int16_t>("-32768", int16_t{-32768});
  shouldParseAndConvert<int16_t>("32767", int16_t{32767});
  shouldParseAndConvert<int16_t>("+32767", int16_t{32767});
  shouldParseAndConvert<int16_t>("3.14", 3);
  shouldNotParse<int16_t>("true");
  shouldNotParse<int16_t>("false");
  shouldNotParse<int16_t>("x42");
  shouldNotParse<int16_t>("42x");
  shouldNotParse<int16_t>("1x5");
  shouldNotConvert<int16_t>("-32769");
  shouldNotConvert<int16_t>("32768");
}

TEST_CASE("parseInteger<int32_t>()") {
  shouldNotParse<int32_t>("");
  shouldParseAndConvert<int32_t>("0", 0);
  shouldParseAndConvert<int32_t>("-2147483648", int32_t{-2147483648});
  shouldParseAndConvert<int32_t>("2147483647", int32_t{2147483647});
  shouldParseAndConvert<int32_t>("+2147483647", int32_t{2147483647});
  shouldParseAndConvert<int32_t>("3.14", 3);
  shouldNotParse<int32_t>("true");
  shouldNotParse<int32_t>("false");
  shouldNotParse<int32_t>("x42");
  shouldNotParse<int32_t>("42x");
  shouldNotParse<int32_t>("1x5");
  shouldNotConvert<int32_t>("-2147483649");
  shouldNotConvert<int32_t>("2147483648");
}

// 5.x branch originally produced T(-1) for negative results when asked for unsigned conversion

TEST_CASE("parseInteger<uint8_t>()") {
  shouldNotParse<uint8_t>("");
  shouldParseAndConvert<uint8_t>("0", 0);
  shouldParseAndConvert<uint8_t>("255", uint8_t{255});
  shouldParseAndConvert<uint8_t>("+255", uint8_t{255});
  shouldParseAndConvert<uint8_t>("3.14", 3);
  shouldNotParse<uint8_t>("true");
  shouldNotParse<uint8_t>("false");
  shouldNotParse<uint8_t>("x42");
  shouldNotParse<uint8_t>("42x");
  shouldNotParse<uint8_t>("1x5");
  shouldNotConvert<uint8_t>("-1");
  shouldNotConvert<uint8_t>("256");
}

TEST_CASE("parseInteger<uint16_t>()") {
  shouldNotParse<uint16_t>("");
  shouldParseAndConvert<uint16_t>("0", 0);
  shouldParseAndConvert<uint16_t>("65535", uint16_t{65535});
  shouldParseAndConvert<uint16_t>("+65535", uint16_t{65535});
  shouldParseAndConvert<uint16_t>("3.14", 3);
  shouldNotParse<uint16_t>("true");
  shouldNotParse<uint16_t>("false");
  shouldNotParse<uint16_t>("x42");
  shouldNotParse<uint16_t>("42x");
  shouldNotParse<uint16_t>("1x5");
  shouldNotConvert<uint16_t>("-1");
  shouldNotConvert<uint16_t>("65536");
}

TEST_CASE("parseInteger<uint32_t>()") {
  shouldNotParse<uint32_t>("");
  shouldParseAndConvert<uint32_t>("0", 0);
  shouldParseAndConvert<uint32_t>("4294967295", uint32_t{4294967295});
  shouldParseAndConvert<uint32_t>("+4294967295", uint32_t{4294967295});
  shouldParseAndConvert<uint32_t>("3.14", 3);
  shouldNotParse<uint32_t>("true");
  shouldNotParse<uint32_t>("false");
  shouldNotParse<uint32_t>("x42");
  shouldNotParse<uint32_t>("42x");
  shouldNotParse<uint32_t>("1x5");
  shouldNotConvert<uint32_t>("-1");
  shouldNotConvert<uint32_t>("4294967296");
}
