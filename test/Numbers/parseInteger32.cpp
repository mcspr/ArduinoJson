// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

// TODO: 7.x modifies namespace all by itself, but tests still have to differentiate themselves
#define ARDUINOJSON_USE_DOUBLE 0
#define ARDUINOJSON_USE_LONG_LONG 0
#define ArduinoJson ArduinoJson_parseInteger32
#include "parseNumber.ipp"

#include <ArduinoJson/Numbers/parseNumber.hpp>
#include <catch.hpp>

#include <string>
#include <cstdint>

#include "parseInteger.ipp" // IWYU pragma: keep

using ArduinoJson::Internals::JsonInteger;
using ArduinoJson::Internals::JsonUnsignedInteger;

// ref. parseInteger.ipp, generates a test case name based on ArduinoJson namespace or its replacement

PARSE_INTEGER_TEST_CASE("parseJsonNumber()") {
  static constexpr auto signed_integer_size = sizeof(JsonInteger);
  STATIC_REQUIRE(signed_integer_size == sizeof(int32_t));

  static constexpr auto unsigned_integer_size = sizeof(JsonUnsignedInteger);
  STATIC_REQUIRE(unsigned_integer_size == sizeof(uint32_t));
}

// {s,u} 8 and 16 are handled in parseInteger.ipp, allowing same tests between the two configurations
// {s,u} 32 and 64 should be tested here

PARSE_INTEGER_TEST_CASE("parseNumber<int32_t>()") {
  SECTION("Min value") {
    static constexpr auto s32_min = std::numeric_limits<int32_t>::min(); // -2147483648
    STATIC_REQUIRE(std::numeric_limits<JsonInteger>::min() == s32_min);

    REQUIRE_PARSE_TYPED_VALUE(<int32_t>, NumberType::SignedInteger, "-2147483648", s32_min);
  }

  SECTION("Max value") {
    static constexpr auto s32_max = std::numeric_limits<int32_t>::max(); // +2147483647
    STATIC_REQUIRE(std::numeric_limits<JsonInteger>::max() == s32_max);

    REQUIRE_PARSE_TYPED_VALUE(<int32_t>, NumberType::UnsignedInteger, "2147483647",  s32_max);
    REQUIRE_PARSE_TYPED_VALUE(<int32_t>, NumberType::UnsignedInteger, "+2147483647", s32_max);
  }

  SECTION("Conversion overflow") {
    REQUIRE_PARSE_TYPED_INCONVERTIBLE(<int32_t>, NumberType::UnsignedInteger, "2147483648");  // u32 -> s32 overflow
    REQUIRE_PARSE_TYPED_CONVERTIBLE(<int32_t>, NumberType::Float, "2147483649.0");  // f32 -> s32 is lossy
    REQUIRE_PARSE_TYPED_CONVERTIBLE(<int32_t>, NumberType::Float, "-2147483649");  // f32 -> s32 is lossy
    REQUIRE_PARSE_TYPED_CONVERTIBLE(<int32_t>, NumberType::Float, "-2147483649.0");  // f32 -> s32 is lossy
  }

  SECTION("Valid number") {
    REQUIRE_PARSE_VALUE(<int32_t>, "0", 0);
    REQUIRE_PARSE_VALUE(<int32_t>, "1234567", 1234567);
    REQUIRE_PARSE_VALUE(<int32_t>, "+1234567", 1234567);
    REQUIRE_PARSE_VALUE(<int32_t>, "-1234567", -1234567);
    REQUIRE_PARSE_VALUE(<int32_t>, "3.14", 3);
    REQUIRE_PARSE_VALUE(<int32_t>, "-3.14", -3);
  }
}

PARSE_INTEGER_TEST_CASE("parseNumber<uint32_t>()") {
  SECTION("Max value") {
    static constexpr auto u32_max = std::numeric_limits<uint32_t>::max(); // 4294967295
    STATIC_REQUIRE(std::numeric_limits<JsonUnsignedInteger>::max() == u32_max);

    REQUIRE_PARSE_TYPED_VALUE(<uint32_t>, NumberType::UnsignedInteger, "4294967295", u32_max);
    REQUIRE_PARSE_TYPED_VALUE(<uint32_t>, NumberType::UnsignedInteger, "+4294967295", u32_max);
  }

  SECTION("Conversion overflow") {
    REQUIRE_PARSE_TYPED_INCONVERTIBLE(<uint32_t>, NumberType::Float, "4294967296");
    REQUIRE_PARSE_TYPED_INCONVERTIBLE(<uint32_t>, NumberType::Float, "12345678900.0");
  }

  SECTION("Conversion sign") {
    REQUIRE_PARSE_TYPED_INCONVERTIBLE(<uint32_t>, NumberType::Float, "-4294967295");
    REQUIRE_PARSE_TYPED_INCONVERTIBLE(<uint32_t>, NumberType::Float, "4294967296");
  }

  SECTION("Valid number") {
    REQUIRE_PARSE_TYPED_VALUE(<uint32_t>, NumberType::UnsignedInteger, "0", 0);
    REQUIRE_PARSE_TYPED_VALUE(<uint32_t>, NumberType::Float, "3.14", 3);
    REQUIRE_PARSE_TYPED_VALUE(<uint32_t>, NumberType::Float, "8388600.0", 8388600);
    REQUIRE_PARSE_TYPED_VALUE(<uint32_t>, NumberType::UnsignedInteger, "12345678", 12345678);
  }
}

PARSE_INTEGER_TEST_CASE("parseNumber<int64_t>()") {
  SECTION("Min value") {
    STATIC_REQUIRE(std::numeric_limits<JsonInteger>::min() >
                   std::numeric_limits<int64_t>::min()); // -9223372036854775808

    REQUIRE_PARSE_TYPED_CONVERTIBLE(<int64_t>, NumberType::Float, "-9223372036854775808");
    REQUIRE_PARSE_TYPED_CONVERTIBLE(<int64_t>, NumberType::Float, "-9223372036854775809");
  }

  SECTION("Max value") {
    STATIC_REQUIRE(std::numeric_limits<JsonInteger>::max() <
                   std::numeric_limits<int64_t>::max()); // +9223372036854775807

    REQUIRE_PARSE_TYPED_CONVERTIBLE(<int64_t>, NumberType::Float, "9223372036854775807");
    REQUIRE_PARSE_TYPED_CONVERTIBLE(<int64_t>, NumberType::Float, "+9223372036854775807");
  }

  SECTION("Valid number") {
    REQUIRE_PARSE_VALUE(<int64_t>, "0", 0);
    REQUIRE_PARSE_VALUE(<int64_t>, "3.14", 3);
    REQUIRE_PARSE_VALUE(<int64_t>, "-3.14", -3);
    REQUIRE_PARSE_VALUE(<int64_t>, "12345678", 12345678);
    REQUIRE_PARSE_VALUE(<int64_t>, "-12345678", -12345678);
  }
}

PARSE_INTEGER_TEST_CASE("parseNumber<uint64_t>()") {
  SECTION("Max value") {
    STATIC_REQUIRE(std::numeric_limits<JsonUnsignedInteger>::max() <
                   std::numeric_limits<uint64_t>::max()); // 18446744073709551615

    REQUIRE_PARSE_TYPED_CONVERTIBLE(<uint64_t>, NumberType::Float, "18446744073709551615");
    REQUIRE_PARSE_TYPED_CONVERTIBLE(<uint64_t>, NumberType::Float, "18446744073709551616");
  }

  SECTION("Valid number") {
    REQUIRE_PARSE_VALUE(<uint64_t>, "0", 0);
    REQUIRE_PARSE_VALUE(<uint64_t>, "3.14", 3);
    REQUIRE_PARSE_VALUE(<uint64_t>, "12345678", 12345678);
  }
}
