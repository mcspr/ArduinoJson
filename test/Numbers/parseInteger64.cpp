// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

// TODO: 7.x modifies namespace all by itself, but tests still have to differentiate themselves
#define ARDUINOJSON_USE_DOUBLE 1
#define ARDUINOJSON_USE_LONG_LONG 1
#define ArduinoJson ArduinoJson_parseInteger64
#include "parseNumber.ipp"

#include <ArduinoJson/Numbers/parseNumber.hpp>
#include <catch.hpp>

#include <string>
#include <cstdint>

#include "parseInteger.ipp" // IWYU pragma: keep

using ArduinoJson::Internals::JsonInteger;
using ArduinoJson::Internals::JsonUnsignedInteger;

// ref. parseInteger.ipp, instead of plain TEST_CASE(...) this *magically* generates a name based on ArduinoJson namespace or its replacement

PARSE_INTEGER_TEST_CASE("parseJsonNumber()") {
  static constexpr auto signed_integer_size = sizeof(JsonInteger);
  STATIC_REQUIRE(signed_integer_size > sizeof(int32_t));

  static constexpr auto unsigned_integer_size = sizeof(JsonUnsignedInteger);
  STATIC_REQUIRE(unsigned_integer_size > sizeof(uint32_t));
}

// {s,u} 8 and 16 are handled in parseInteger.ipp, allowing same tests between the two configurations
// {s,u} 32 and 64 should be tested here

PARSE_INTEGER_TEST_CASE("parseNumber<int32_t>()") {
  SECTION("Min value") {
    static constexpr auto s32_min = std::numeric_limits<int32_t>::min(); // -2147483648
    STATIC_REQUIRE(std::numeric_limits<JsonInteger>::min() < s32_min);

    REQUIRE_PARSE_TYPED_VALUE(<int32_t>, NumberType::SignedInteger, "-2147483648", s32_min);
    REQUIRE_PARSE_TYPED_INCONVERTIBLE(<int32_t>, NumberType::SignedInteger, "-2147483649");
  }

  SECTION("Max value") {
    static constexpr auto s32_max = std::numeric_limits<int32_t>::max(); // +2147483647
    STATIC_REQUIRE(std::numeric_limits<JsonInteger>::max() > s32_max);

    REQUIRE_PARSE_TYPED_VALUE(<int32_t>, NumberType::UnsignedInteger, "2147483647", s32_max);
    REQUIRE_PARSE_TYPED_VALUE(<int32_t>, NumberType::UnsignedInteger, "+2147483647", s32_max);

    REQUIRE_PARSE_TYPED_INCONVERTIBLE(<int32_t>, NumberType::UnsignedInteger, "2147483648");
  }

  SECTION("Valid number") {
    REQUIRE_PARSE_TYPED_VALUE(<int32_t>, NumberType::UnsignedInteger, "0", 0);
    REQUIRE_PARSE_TYPED_VALUE(<int32_t>, NumberType::UnsignedInteger, "+1234567", 1234567);
    REQUIRE_PARSE_TYPED_VALUE(<int32_t>, NumberType::SignedInteger, "-1234567", -1234567);
    REQUIRE_PARSE_TYPED_VALUE(<int32_t>, NumberType::Float, "3.14", 3);
    REQUIRE_PARSE_TYPED_VALUE(<int32_t>, NumberType::Float, "-3.14", -3);
  }
}

PARSE_INTEGER_TEST_CASE("parseNumber<uint32_t>()") {
  SECTION("Max value") {
    static constexpr auto u32_max = std::numeric_limits<uint32_t>::max(); // 4294967295
    STATIC_REQUIRE(std::numeric_limits<JsonUnsignedInteger>::max() > u32_max);

    REQUIRE_PARSE_TYPED_VALUE(<uint32_t>, NumberType::UnsignedInteger, "4294967295", u32_max);
    REQUIRE_PARSE_TYPED_VALUE(<uint32_t>, NumberType::UnsignedInteger, "+4294967295", u32_max);

    REQUIRE_PARSE_TYPED_INCONVERTIBLE(<int32_t>, NumberType::UnsignedInteger, "4294967296");
  }

  SECTION("Valid number") {
    REQUIRE_PARSE_TYPED_VALUE(<uint32_t>, NumberType::UnsignedInteger, "0", 0);
    REQUIRE_PARSE_TYPED_VALUE(<uint32_t>, NumberType::Float, "3.14", 3);
    REQUIRE_PARSE_TYPED_VALUE(<uint32_t>, NumberType::Float, "838860.0", 838860);
    REQUIRE_PARSE_TYPED_VALUE(<uint32_t>, NumberType::Double, "12345678.0", 12345678);
    REQUIRE_PARSE_TYPED_VALUE(<uint32_t>, NumberType::UnsignedInteger, "12345678", 12345678);
  }
}

PARSE_INTEGER_TEST_CASE("parseNumber<int64_t>()") {
  SECTION("Min value") {
    static constexpr auto s64_min = std::numeric_limits<int64_t>::min(); // -9223372036854775808
    STATIC_REQUIRE(std::numeric_limits<JsonInteger>::min() == s64_min);

    REQUIRE_PARSE_TYPED_VALUE(<int64_t>, NumberType::SignedInteger, "-9223372036854775808", s64_min);
    REQUIRE_PARSE_TYPED_CONVERTIBLE(<int64_t>, NumberType::Double, "-9223372036854775809");  // f64 -> s64 is lossy
  }

  SECTION("Max value") {
    static constexpr auto s64_max = std::numeric_limits<int64_t>::max(); // +9223372036854775807
    STATIC_REQUIRE(std::numeric_limits<JsonInteger>::max() == s64_max);

    REQUIRE_PARSE_TYPED_VALUE(<int64_t>, NumberType::UnsignedInteger, "9223372036854775807", s64_max);
    REQUIRE_PARSE_TYPED_VALUE(<int64_t>, NumberType::UnsignedInteger, "+9223372036854775807", s64_max);
    REQUIRE_PARSE_TYPED_INCONVERTIBLE(<int64_t>, NumberType::UnsignedInteger, "9223372036854775808");
  }

  SECTION("Valid number") {
    REQUIRE_PARSE_VALUE(<int64_t>, "0", 0);
    REQUIRE_PARSE_VALUE(<int64_t>, "3.14", 3);
    REQUIRE_PARSE_VALUE(<int64_t>, "12345678.0", 12345678);
    REQUIRE_PARSE_VALUE(<int64_t>, "12345678", 12345678);
  }
}

PARSE_INTEGER_TEST_CASE("parseNumber<uint64_t>()") {
  SECTION("Max value") {
    static constexpr auto u64_max = std::numeric_limits<uint64_t>::max(); // 18446744073709551615
    STATIC_REQUIRE(std::numeric_limits<JsonUnsignedInteger>::max() == u64_max);

    REQUIRE_PARSE_TYPED_VALUE(<uint64_t>, NumberType::UnsignedInteger, "18446744073709551615", u64_max);
    REQUIRE_PARSE_TYPED_VALUE(<uint64_t>, NumberType::UnsignedInteger, "+18446744073709551615", u64_max);

    REQUIRE_PARSE_TYPED_INCONVERTIBLE(<uint64_t>, NumberType::Double, "18446744073709551616");  // f64 -> u64 overflow
  }

  SECTION("Valid number") {
    REQUIRE_PARSE_VALUE(<uint64_t>, "0", 0);
    REQUIRE_PARSE_VALUE(<uint64_t>, "3.14", 3);
    REQUIRE_PARSE_VALUE(<uint64_t>, "12345678.0", 12345678);
    REQUIRE_PARSE_VALUE(<uint64_t>, "12345678", 12345678);
  }
}
