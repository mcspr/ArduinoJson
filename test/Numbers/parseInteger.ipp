// MIT License

#pragma once

#include <ArduinoJson/Numbers/parseNumber.hpp>
#include "parseNumber.ipp"

#include <catch.hpp>

#define XPARSE_INTEGER_TEST_STRINGIFY(X) #X
#define PARSE_INTEGER_TEST_STRINGIFY(X) XPARSE_INTEGER_TEST_STRINGIFY(X)

#define PARSE_INTEGER_TEST_NAME(X) PARSE_INTEGER_TEST_STRINGIFY(ArduinoJson) "::" X
#define PARSE_INTEGER_TEST_CASE(X) TEST_CASE(PARSE_INTEGER_TEST_NAME(X))
#define PARSE_INTEGER_TEMPLATE_TEST_CASE(X, ...) TEMPLATE_TEST_CASE(PARSE_INTEGER_TEST_NAME(X), __VA_ARGS__)

// nb. 5.x branch originally produced T(-1) for negative results when asked for unsigned conversion

namespace parse_integer_common_tests {
namespace {

PARSE_INTEGER_TEST_CASE("parseJsonNumber() common tests") {
  SECTION("Type") {
    REQUIRE_PARSE_TYPE(NumberType::UnsignedInteger, "0");
    REQUIRE_PARSE_TYPE(NumberType::SignedInteger, "-0");
    REQUIRE_PARSE_TYPE(NumberType::UnsignedInteger, "+0");

    REQUIRE_PARSE_TYPE(NumberType::UnsignedInteger, "1");
    REQUIRE_PARSE_TYPE(NumberType::SignedInteger, "-1");
    REQUIRE_PARSE_TYPE(NumberType::UnsignedInteger, "+1");

    REQUIRE_PARSE_TYPE(NumberType::Invalid, ".");
    REQUIRE_PARSE_TYPE(NumberType::Invalid, "...");
    REQUIRE_PARSE_TYPE(NumberType::Invalid, "---");
    REQUIRE_PARSE_TYPE(NumberType::Invalid, "+++");
  }

  SECTION("Positive") {
    static constexpr const char s[] = "42";

    REQUIRE_PARSE_VALUE(<int8_t>, s, 42);
    REQUIRE_PARSE_VALUE(<int16_t>, s, 42);
    REQUIRE_PARSE_VALUE(<int32_t>, s, 42);
    REQUIRE_PARSE_VALUE(<int64_t>, s, 42);

    REQUIRE_PARSE_VALUE(<uint8_t>, s, 42);
    REQUIRE_PARSE_VALUE(<uint16_t>, s, 42);
    REQUIRE_PARSE_VALUE(<uint32_t>, s, 42);
    REQUIRE_PARSE_VALUE(<uint64_t>, s, 42);

    REQUIRE_PARSE_VALUE(<float>, s, 42.0f);
    REQUIRE_PARSE_VALUE(<double>, s, 42.0);
  }

  SECTION("Negative") {
    static constexpr const char s[] = "-42";

    REQUIRE_PARSE_VALUE(<int8_t>, s, -42);
    REQUIRE_PARSE_VALUE(<int16_t>, s, -42);
    REQUIRE_PARSE_VALUE(<int32_t>, s, -42);
    REQUIRE_PARSE_VALUE(<int64_t>, s, -42);

    REQUIRE_PARSE_INCONVERTIBLE(<uint8_t>, s);
    REQUIRE_PARSE_INCONVERTIBLE(<uint16_t>, s);
    REQUIRE_PARSE_INCONVERTIBLE(<uint32_t>, s);
    REQUIRE_PARSE_INCONVERTIBLE(<uint64_t>, s);

    REQUIRE_PARSE_VALUE(<float>, s, -42.0f);
    REQUIRE_PARSE_VALUE(<double>, s, -42.0);
  }
}

PARSE_INTEGER_TEST_CASE("parseNumber<int8_t>()") {
  SECTION("Min and Max") {
    static constexpr auto s8_min = std::numeric_limits<int8_t>::min();
    static constexpr auto s8_max = std::numeric_limits<int8_t>::max();

    REQUIRE_PARSE_VALUE(<int8_t>, "-128", s8_min);
    REQUIRE_PARSE_VALUE(<int8_t>, "127", s8_max);
    REQUIRE_PARSE_VALUE(<int8_t>, "+127", s8_max);
  }

  SECTION("Conversion overflow") {
    REQUIRE_PARSE_INCONVERTIBLE(<int8_t>, "128");
    REQUIRE_PARSE_INCONVERTIBLE(<int8_t>, "-129");
    REQUIRE_PARSE_INCONVERTIBLE(<int8_t>, "128.0");
    REQUIRE_PARSE_INCONVERTIBLE(<int8_t>, "-129.0");
  }
}

PARSE_INTEGER_TEST_CASE("parseNumber<int16_t>()") {
  SECTION("Min and Max") {
    static constexpr auto s16_min = std::numeric_limits<int16_t>::min();
    static constexpr auto s16_max = std::numeric_limits<int16_t>::max();

    REQUIRE_PARSE_VALUE(<int16_t>, "-32768", s16_min);
    REQUIRE_PARSE_VALUE(<int16_t>, "32767", s16_max);
    REQUIRE_PARSE_VALUE(<int16_t>, "+32767", s16_max);
  }

  SECTION("Conversion overflow") {
    REQUIRE_PARSE_INCONVERTIBLE(<int16_t>, "32768");
    REQUIRE_PARSE_INCONVERTIBLE(<int16_t>, "-32769");
    REQUIRE_PARSE_INCONVERTIBLE(<int16_t>, "32768.0");
    REQUIRE_PARSE_INCONVERTIBLE(<int16_t>, "-32769.0");
  }

  SECTION("Valid number") {
    REQUIRE_PARSE_VALUE(<int16_t>, "12345", 12345);
    REQUIRE_PARSE_VALUE(<int16_t>, "+12345", 12345);
    REQUIRE_PARSE_VALUE(<int16_t>, "-12345", -12345);
    REQUIRE_PARSE_VALUE(<int16_t>, "3.14", 3);
    REQUIRE_PARSE_VALUE(<int16_t>, "-3.14", -3);
  }
}

PARSE_INTEGER_TEST_CASE("parseNumber<uint8_t>()") {
  SECTION("Min and Max") {
    static constexpr auto u8_min = std::numeric_limits<uint8_t>::min();
    static constexpr auto u8_max = std::numeric_limits<uint8_t>::max();

    REQUIRE_PARSE_VALUE(<uint8_t>, "0", u8_min);
    REQUIRE_PARSE_VALUE(<uint8_t>, "255", u8_max);
    REQUIRE_PARSE_VALUE(<uint8_t>, "+255", u8_max);
  }

  SECTION("Conversion overflow") {
    REQUIRE_PARSE_INCONVERTIBLE(<uint8_t>, "256");  // should request a larger type
    REQUIRE_PARSE_INCONVERTIBLE(<uint8_t>, "300");
    REQUIRE_PARSE_INCONVERTIBLE(<uint8_t>, "-300");
    REQUIRE_PARSE_INCONVERTIBLE(<uint8_t>, "300.0");
    REQUIRE_PARSE_INCONVERTIBLE(<uint8_t>, "-300.0");
  }

  SECTION("Valid number") {
    REQUIRE_PARSE_VALUE(<uint8_t>, "0", 0);
    REQUIRE_PARSE_VALUE(<uint8_t>, "123", 123);
    REQUIRE_PARSE_VALUE(<uint8_t>, "+123", 123);
    REQUIRE_PARSE_VALUE(<uint8_t>, "3.14", 3);
  }
}

PARSE_INTEGER_TEST_CASE("parseNumber<uint16_t>()") {
  SECTION("Parse as unsigned") {
    static constexpr auto u16_min = std::numeric_limits<uint16_t>::min();
    static constexpr auto u16_max = std::numeric_limits<uint16_t>::max();

    REQUIRE_PARSE_VALUE(<uint16_t>, "0", u16_min);
    REQUIRE_PARSE_VALUE(<uint16_t>, "65535", u16_max);
    REQUIRE_PARSE_VALUE(<uint16_t>, "+65535", u16_max);
  }

  SECTION("Conversion overflow") {
    REQUIRE_PARSE_INCONVERTIBLE(<uint16_t>, "65536");  // should request a larger type
    REQUIRE_PARSE_INCONVERTIBLE(<uint16_t>, "100000");
    REQUIRE_PARSE_INCONVERTIBLE(<uint16_t>, "-100000");
    REQUIRE_PARSE_INCONVERTIBLE(<uint16_t>, "100000.0");
    REQUIRE_PARSE_INCONVERTIBLE(<uint16_t>, "-100000.0");
  }

  SECTION("Valid number") {
    REQUIRE_PARSE_VALUE(<uint16_t>, "0", 0);
    REQUIRE_PARSE_VALUE(<uint16_t>, "3.14", 3);
    REQUIRE_PARSE_VALUE(<uint16_t>, "12345.0", 12345);
    REQUIRE_PARSE_VALUE(<uint16_t>, "12345", 12345);
  }
}

// type conversion should never produce usable result w/ invalid input
PARSE_INTEGER_TEMPLATE_TEST_CASE("parseNumber<TestType>() invalid input",
        "[template][parseInteger][parseNumber]",
        int8_t, int16_t, int32_t, int64_t,
        uint8_t, uint16_t, uint32_t, uint64_t)
{
  REQUIRE_PARSE_FAIL(<TestType>, "");
  REQUIRE_PARSE_FAIL(<TestType>, "null");
  REQUIRE_PARSE_FAIL(<TestType>, "true");
  REQUIRE_PARSE_FAIL(<TestType>, "false");
  REQUIRE_PARSE_FAIL(<TestType>, "\"\"");
  REQUIRE_PARSE_FAIL(<TestType>, "''");
  REQUIRE_PARSE_FAIL(<TestType>, "\"123\"");
  REQUIRE_PARSE_FAIL(<TestType>, "'123'");
  REQUIRE_PARSE_FAIL(<TestType>, "1x5");
  REQUIRE_PARSE_FAIL(<TestType>, "42x");
  REQUIRE_PARSE_FAIL(<TestType>, "6a3");
  REQUIRE_PARSE_FAIL(<TestType>, "x42");
}

// type conversion should always produce zero w/ valid input of any type
PARSE_INTEGER_TEMPLATE_TEST_CASE("parseNumber<TestType>() zero",
        "[template][parseInteger][parseNumber]",
        int8_t, int16_t, int32_t, int64_t,
        uint8_t, uint16_t, uint32_t, uint64_t)
{
  REQUIRE_PARSE_VALUE(<TestType>, "0", 0);
  REQUIRE_PARSE_VALUE(<TestType>, "0.0", 0);
  REQUIRE_PARSE_VALUE(<TestType>, "-0", 0);
  REQUIRE_PARSE_VALUE(<TestType>, "-0.0", 0);
}

// signed conversion should always retain the sign
PARSE_INTEGER_TEMPLATE_TEST_CASE("parseNumber<TestType>() signed conversion",
        "[template][parseInteger][parseNumber]",
        int8_t, int16_t, int32_t, int64_t)
{
  REQUIRE_PARSE_VALUE(<TestType>, "1", 1);
  REQUIRE_PARSE_VALUE(<TestType>, "-1", -1);
  REQUIRE_PARSE_VALUE(<TestType>, "+1", 1);
  REQUIRE_PARSE_VALUE(<TestType>, "3.14", 3);
  REQUIRE_PARSE_VALUE(<TestType>, "+3.14", 3);
  REQUIRE_PARSE_VALUE(<TestType>, "-3.14", -3);
}

// unsigned conversion should fail
PARSE_INTEGER_TEMPLATE_TEST_CASE("parseNumber<TestType>() unsigned conversion",
        "[template][parseInteger][parseNumber]",
        uint8_t, uint16_t, uint32_t, uint64_t)
{
    REQUIRE_PARSE_VALUE(<TestType>, "1", 1);
    REQUIRE_PARSE_VALUE(<TestType>, "+1", 1);

    REQUIRE_PARSE_VALUE(<TestType>, "123", 123);
    REQUIRE_PARSE_VALUE(<TestType>, "+123", 123);

    REQUIRE_PARSE_VALUE(<TestType>, "3.14", 3);
    REQUIRE_PARSE_VALUE(<TestType>, "+3.14", 3);

    REQUIRE_PARSE_INCONVERTIBLE(<TestType>, "-1");
    REQUIRE_PARSE_INCONVERTIBLE(<TestType>, "-123");
    REQUIRE_PARSE_INCONVERTIBLE(<TestType>, "-3.14");
}

}
}
