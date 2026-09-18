// MIT License

#pragma once

#include <ArduinoJson/Numbers/parseNumber.hpp>
#include "parseNumber.ipp"

#include <catch.hpp>
#include <limits>

#define XPARSE_FLOAT_TEST_STRINGIFY(X) #X
#define PARSE_FLOAT_TEST_STRINGIFY(X) XPARSE_FLOAT_TEST_STRINGIFY(X)

#define PARSE_FLOAT_TEST_NAME(X) PARSE_FLOAT_TEST_STRINGIFY(ArduinoJson) "::" X
#define PARSE_FLOAT_TEST_CASE(X) TEST_CASE(PARSE_FLOAT_TEST_NAME(X))
#define PARSE_FLOAT_TEMPLATE_TEST_CASE(X, ...) TEMPLATE_TEST_CASE(PARSE_FLOAT_TEST_NAME(X), __VA_ARGS__)

using ArduinoJson::Internals::FloatTraits;
using ArduinoJson::Internals::JsonFloat;

namespace parse_float_common_tests {
namespace {

PARSE_FLOAT_TEST_CASE("parseJsonNumber() common tests") {
  SECTION("Type") {
    REQUIRE_PARSE_TYPE(NumberType::Float, "0.");
    REQUIRE_PARSE_TYPE(NumberType::Float, "-0.");
    REQUIRE_PARSE_TYPE(NumberType::Float, "+0.");

    REQUIRE_PARSE_TYPE(NumberType::Float, "0.0");
    REQUIRE_PARSE_TYPE(NumberType::Float, "-0.0");
    REQUIRE_PARSE_TYPE(NumberType::Float, "+0.0");

    REQUIRE_PARSE_TYPE(NumberType::Float, "1.0");
    REQUIRE_PARSE_TYPE(NumberType::Float, "-1.0");
    REQUIRE_PARSE_TYPE(NumberType::Float, "+1.0");
  }

  SECTION("Positive") {
    static constexpr const char s[] = "3.14";

    REQUIRE_PARSE_VALUE(<int8_t>, s, 3);
    REQUIRE_PARSE_VALUE(<int16_t>, s, 3);
    REQUIRE_PARSE_VALUE(<int32_t>, s, 3);
    REQUIRE_PARSE_VALUE(<int64_t>, s, 3);

    REQUIRE_PARSE_VALUE(<uint8_t>, s, 3);
    REQUIRE_PARSE_VALUE(<uint16_t>, s, 3);
    REQUIRE_PARSE_VALUE(<uint32_t>, s, 3);
    REQUIRE_PARSE_VALUE(<uint64_t>, s, 3);

    REQUIRE_PARSE_VALUE(<float>, s, 3.14f);
    REQUIRE_PARSE_VALUE(<double>, s, 3.14);
  }

  SECTION("Negative") {
    static constexpr const char s[] = "-3.14";

    REQUIRE_PARSE_VALUE(<int8_t>, s, -3);
    REQUIRE_PARSE_VALUE(<int16_t>, s, -3);
    REQUIRE_PARSE_VALUE(<int32_t>, s, -3);
    REQUIRE_PARSE_VALUE(<int64_t>, s, -3);

    REQUIRE_PARSE_INCONVERTIBLE(<uint8_t>, s);
    REQUIRE_PARSE_INCONVERTIBLE(<uint16_t>, s);
    REQUIRE_PARSE_INCONVERTIBLE(<uint32_t>, s);
    REQUIRE_PARSE_INCONVERTIBLE(<uint64_t>, s);

    REQUIRE_PARSE_VALUE(<float>, s, -3.14f);
    REQUIRE_PARSE_VALUE(<double>, s, -3.14);
  }

  SECTION("String bounds") {
    static constexpr const char s[4] = {'4', '2', '.', '0'};
    const auto result = parseJsonNumber(&s[0], sizeof(s));
    CHECK(result.value.type() == NumberType::Float);

    const auto s8 = result.convertTo<int8_t>();
    CHECK(s8);
    REQUIRE(s8.value == 42);

    const auto u8 = result.convertTo<uint8_t>();
    CHECK(u8);
    REQUIRE(u8.value == 42);

    const auto f32 = result.convertTo<float>();
    CHECK(f32);
    REQUIRE(f32.value == Approx(42.0f));

    const auto f64 = result.convertTo<double>();
    CHECK(f64);
    REQUIRE(f64.value == Approx(42.0));
  }
}

PARSE_FLOAT_TEMPLATE_TEST_CASE("parseNumber<TestType>() common",
        "[template][parseFloat][parseDouble][parseNumber]",
        float, double)
{
  SECTION("Zero") {
    static constexpr auto positive_zero = TestType();
    static constexpr auto negative_zero = -TestType();

    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::UnsignedInteger, "0", positive_zero);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::UnsignedInteger, "+0", positive_zero);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::SignedInteger, "-0", positive_zero);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "0.", positive_zero);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "+0.", positive_zero);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "-0.", negative_zero);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "0.0", positive_zero);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "+0.0", positive_zero);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "-0.0", negative_zero);
  }

  SECTION("No exponent") {
    static constexpr auto value = TestType{ 3.14f };

    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "3.14", value);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "-3.14", -value);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "+3.14", +value);
  }

  SECTION("No decimal point") {
    static constexpr auto positive_exponent = TestType{ 1e+38f };

    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "1E+38", positive_exponent);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "1e+38", positive_exponent);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "-1E+38", -positive_exponent);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "-1e+38", -positive_exponent);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "+1E+38", +positive_exponent);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "+1e+38", +positive_exponent);

    static constexpr auto negative_exponent = TestType{ 1e-38f };

    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "1E-38", negative_exponent);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "1e-38", negative_exponent);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "-1E-38", -negative_exponent);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "-1e-38", -negative_exponent);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "+1E-38", +negative_exponent);
    REQUIRE_PARSE_TYPED_VALUE(<TestType>, NumberType::Float, "+1e-38", +negative_exponent);
  }

  SECTION("NaN") {
    static constexpr auto nan = std::numeric_limits<TestType>::quiet_NaN();

    REQUIRE_PARSE_VALUE(<TestType>, "NaN", nan);
    REQUIRE_PARSE_VALUE(<TestType>, "nan", nan);
    REQUIRE_PARSE_VALUE(<TestType>, "-NaN", nan);
    REQUIRE_PARSE_VALUE(<TestType>, "+NaN", +nan);
    REQUIRE_PARSE_VALUE(<TestType>, "-nan", nan);
    REQUIRE_PARSE_VALUE(<TestType>, "+nan", +nan);
  }

  SECTION("Infinity") {
    static constexpr auto inf = std::numeric_limits<TestType>::infinity();

    REQUIRE_PARSE_VALUE(<TestType>, "Infinity", inf);
    REQUIRE_PARSE_VALUE(<TestType>, "+Infinity", inf);
    REQUIRE_PARSE_VALUE(<TestType>, "-Infinity", -inf);
    REQUIRE_PARSE_VALUE(<TestType>, "inf", inf);
    REQUIRE_PARSE_VALUE(<TestType>, "+inf", inf);
    REQUIRE_PARSE_VALUE(<TestType>, "-inf", -inf);
  }
}

PARSE_FLOAT_TEMPLATE_TEST_CASE("parseNumber<TestType>() invalid inputs",
        "[template][parseFloat][parseDouble][parseNumber]",
        float, double)
{
  SECTION("Empty") {
    REQUIRE_PARSE_FAIL(<TestType>, "");
  }

  SECTION("null") {
    REQUIRE_PARSE_FAIL(<TestType>, "null");
  }

  SECTION("Boolean") {
    REQUIRE_PARSE_FAIL(<TestType>, "false");
    REQUIRE_PARSE_FAIL(<TestType>, "true");
  }

  SECTION("Empty string") {
    REQUIRE_PARSE_FAIL(<TestType>, "\"\"");
    REQUIRE_PARSE_FAIL(<TestType>, "''");
  }

  SECTION("Quoted string") {
    REQUIRE_PARSE_FAIL(<TestType>, "\"1.2345\"");
    REQUIRE_PARSE_FAIL(<TestType>, "'1.2345'");
  }

  SECTION("Invalid string") {
    REQUIRE_PARSE_FAIL(<TestType>, "6a3");
    REQUIRE_PARSE_FAIL(<TestType>, "-.");
    REQUIRE_PARSE_FAIL(<TestType>, "+.");
    REQUIRE_PARSE_FAIL(<TestType>, ".");
  }

  SECTION("NaN capitalization") {
    REQUIRE_PARSE_FAIL(<TestType>, "Nan");
    REQUIRE_PARSE_FAIL(<TestType>, "nAn");
    REQUIRE_PARSE_FAIL(<TestType>, "nAN");
    REQUIRE_PARSE_FAIL(<TestType>, "NAN");
    REQUIRE_PARSE_FAIL(<TestType>, "n");
    REQUIRE_PARSE_FAIL(<TestType>, "N");
  }

  SECTION("Infinity capitalization") {
    REQUIRE_PARSE_FAIL(<TestType>, "Inf");
    REQUIRE_PARSE_FAIL(<TestType>, "iNF");
    REQUIRE_PARSE_FAIL(<TestType>, "INF");
    REQUIRE_PARSE_FAIL(<TestType>, "infinity");
    REQUIRE_PARSE_FAIL(<TestType>, "I");
    REQUIRE_PARSE_FAIL(<TestType>, "i");
  }
}

PARSE_FLOAT_TEMPLATE_TEST_CASE("parseNumber<TestType>() exponent overflow",
        "[template][parseFloat][parseDouble][parseNumber]",
        float, double)
{
  // generate a large string with a lot of digits in either decimal part, fractional part, or both
  // current parser implementation relies on
  // a) counting encountered digits to calculate total exponent
  // b) parsing explicitly specified exponent value (digits after e or E)
  // c) verifying counted exponent against ieee 754 natural limits
  //
  // parsing a very large number should always fail at parsing, since the underlying type would not hold it
  // even before the conversion utility func would attempt to check for conversion result

  static constexpr size_t exponent_type_max =
    std::numeric_limits<typename FloatTraits<JsonFloat>::exponent_type>::max();

  SECTION("Exponent overflow (Issue #2220)") {
    std::string s;
    s.reserve(2 * (exponent_type_max + 16));

    SECTION("Integral part") {  // 1 000 0...
      s.append("1");
      s.append(exponent_type_max + 16, '0');
      REQUIRE_PARSE_FAIL(<float>, s.c_str());
      REQUIRE_PARSE_FAIL(<double>, s.c_str());
    }

    SECTION("Decimal part") {  // 0.000 0... 1
      s.append("0.0");
      s.append(exponent_type_max + 16, '0');
      s.append("1");
      REQUIRE_PARSE_FAIL(<float>, s.c_str());
      REQUIRE_PARSE_FAIL(<double>, s.c_str());
    }

    SECTION("Integral and decimal parts") {  // 0 000... 0.0 000... 1
      s.append(exponent_type_max + 16, '0');
      s.append(".0");
      s.append(exponent_type_max + 16, '0');
      s.append("1");
      REQUIRE_PARSE_FAIL(<float>, s.c_str());
      REQUIRE_PARSE_FAIL(<double>, s.c_str());
    }
  }
}

}
}
