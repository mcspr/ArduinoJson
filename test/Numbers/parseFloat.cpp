// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

// TODO: 7.x modifies namespace all by itself
#define ARDUINOJSON_USE_DOUBLE 0
#define ArduinoJson ArduinoJson_parseFloat
#include "parseNumber.ipp"

#include <ArduinoJson/Numbers/parseNumber.hpp>
#include <ArduinoJson/TypeTraits/FloatTraits.hpp>
#include <ArduinoJson/Polyfills/math.hpp>

#include <catch.hpp>

#include "parseFloatDouble.ipp"  // IWYU pragma: keep

PARSE_FLOAT_TEST_CASE("parseNumber<T>()") {
  STATIC_REQUIRE(std::is_same<ArduinoJson::Internals::JsonFloat, float>::value);
}

PARSE_FLOAT_TEST_CASE("parseJsonNumber()") {
  SECTION("Floating point type") {
    REQUIRE_PARSE_TYPE(NumberType::Float, "3.402823e38");
    REQUIRE_PARSE_TYPE(NumberType::Invalid, "1.7976931348623157e308");
  }

  SECTION("NaN type") {  // special value always assigned as JsonFloat type
    REQUIRE_PARSE_TYPE(NumberType::Float, "NaN");
    REQUIRE_PARSE_TYPE(NumberType::Float, "nan");
    REQUIRE_PARSE_TYPE(NumberType::Float, "-NaN");
    REQUIRE_PARSE_TYPE(NumberType::Float, "+NaN");
    REQUIRE_PARSE_TYPE(NumberType::Float, "-nan");
    REQUIRE_PARSE_TYPE(NumberType::Float, "+nan");
  }

  SECTION("Infinity type") {  // special value always assigned as JsonFloat type
    REQUIRE_PARSE_TYPE(NumberType::Float, "Infinity");
    REQUIRE_PARSE_TYPE(NumberType::Float, "+Infinity");
    REQUIRE_PARSE_TYPE(NumberType::Float, "-Infinity");
    REQUIRE_PARSE_TYPE(NumberType::Float, "inf");
    REQUIRE_PARSE_TYPE(NumberType::Float, "+inf");
    REQUIRE_PARSE_TYPE(NumberType::Float, "-inf");
  }
}

PARSE_FLOAT_TEST_CASE("parseNumber<float>()") {
  SECTION("Max value") {
    static constexpr auto max_value = std::numeric_limits<float>::max();

    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "340.2823e+36", max_value);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "34.02823e+37", max_value);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "3.402823e+38", max_value);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "0.3402823e+39", max_value);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "0.03402823e+40", max_value);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "0.003402823e+41", max_value);
  }

  SECTION("Min value") {
    static constexpr auto min_value = std::numeric_limits<float>::min();

    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "117.5494e-40", min_value);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "11.75494e-39", min_value);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "1.175494e-38", min_value);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "0.1175494e-37", min_value);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "0.01175494e-36", min_value);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "0.001175494e-35", min_value);
  }

  SECTION("Very long") {
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "9.99999999999999999999999", 10.0f);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "-9.99999999999999999999999", -10.0f);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "999999999999999999999999", 1e+24f);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "-999999999999999999999999", -1e+24f);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "0.00000000000000000000000000000001", 1e-32f);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "100000000000000000000000000000000.0", 1e+32f);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "100000000000000000000000000000000.00000000000000000000000000000", 1e+32f);
  }

  SECTION("Mantissa too long to fit") {
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "0.340282346638528861111111111111", 0.34028234663852886f);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "34028234663852886.11111111111111", 34028234663852886.0f);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "34028234.66385288611111111111111", 34028234.663852886f);

    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "-0.340282346638528861111111111111", -0.34028234663852886f);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "-34028234663852886.11111111111111", -34028234663852886.0f);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "-34028234.66385288611111111111111", -34028234.663852886f);
  }

  SECTION("Exponent too big") {
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "1e38", 1e38f);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "-1e38", -1e38f);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "1e-45", 1e-45f);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "-1e-45", -1e-45f);
    REQUIRE_PARSE_TYPED_VALUE(<float>, NumberType::Float, "12345678.0", 1.234568e7f);

    REQUIRE_PARSE_FAIL(<float>, "1e-46");
    REQUIRE_PARSE_FAIL(<float>, "1e39");
    REQUIRE_PARSE_FAIL(<float>, "-1e39");
    REQUIRE_PARSE_FAIL(<float>, "1e-100");
    REQUIRE_PARSE_FAIL(<float>, "-1e100");
    REQUIRE_PARSE_FAIL(<float>, "1e100");
    REQUIRE_PARSE_FAIL(<float>, "1e255");
    REQUIRE_PARSE_FAIL(<float>, "1e-255");
  }
}

PARSE_FLOAT_TEST_CASE("parseNumber<double>()") {
  // USE_DOUBLE = 0 parser only accepts numbers that fit in a float32, 2nd part is simply converting float -> double

  SECTION("Max value") {
    REQUIRE_PARSE_FAIL(<double>, ".017976931348623147e+310");
    REQUIRE_PARSE_FAIL(<double>, ".17976931348623147e+309");
    REQUIRE_PARSE_FAIL(<double>, "1.7976931348623147e+308");
    REQUIRE_PARSE_FAIL(<double>, "17.976931348623147e+307");
    REQUIRE_PARSE_FAIL(<double>, "179.76931348623147e+306");
  }

  SECTION("Min value") {
    REQUIRE_PARSE_FAIL(<double>, ".022250738585072014e-306");
    REQUIRE_PARSE_FAIL(<double>, ".22250738585072014e-307");
    REQUIRE_PARSE_FAIL(<double>, "2.2250738585072014e-308");
    REQUIRE_PARSE_FAIL(<double>, "22.250738585072014e-309");
    REQUIRE_PARSE_FAIL(<double>, "222.50738585072014e-310");
  }

  SECTION("Very long string") {
    REQUIRE_PARSE_TYPED_VALUE(<double>, NumberType::Float, "9.99999999999999999999999", 10.0);
    REQUIRE_PARSE_TYPED_VALUE(<double>, NumberType::Float, "-9.99999999999999999999999", -10.0);
    REQUIRE_PARSE_TYPED_VALUE(<double>, NumberType::Float, "999999999999999999999999", 1e+24);
    REQUIRE_PARSE_TYPED_VALUE(<double>, NumberType::Float, "-999999999999999999999999", -1e+24);
    REQUIRE_PARSE_TYPED_VALUE(<double>, NumberType::Float, "0.00000000000000000000000000000001", 1e-32);
    REQUIRE_PARSE_TYPED_VALUE(<double>, NumberType::Float, "100000000000000000000000000000000.0", 1e+32);
    REQUIRE_PARSE_TYPED_VALUE(<double>, NumberType::Float, "100000000000000000000000000000000.00000000000000000000000000000", 1e+32);
  }

  SECTION("Mantissa too long to fit") {
    REQUIRE_PARSE_TYPED_VALUE(<double>, NumberType::Float, "0.179769313486231571111111111111", 0.17976931348623157);
    REQUIRE_PARSE_TYPED_VALUE(<double>, NumberType::Float, "17976931348623157.11111111111111", 17976931348623157.0);
    REQUIRE_PARSE_TYPED_VALUE(<double>, NumberType::Float, "1797693.134862315711111111111111", 1797693.1348623157);

    REQUIRE_PARSE_TYPED_VALUE(<double>, NumberType::Float, "-0.179769313486231571111111111111", -0.17976931348623157);
    REQUIRE_PARSE_TYPED_VALUE(<double>, NumberType::Float, "-17976931348623157.11111111111111", -17976931348623157.0);
    REQUIRE_PARSE_TYPED_VALUE(<double>, NumberType::Float, "-1797693.134862315711111111111111", -1797693.1348623157);
  }

  SECTION("Exponent too big") {
    REQUIRE_PARSE_FAIL(<double>, "1e39");
    REQUIRE_PARSE_FAIL(<double>, "-1e39");

    REQUIRE_PARSE_FAIL(<double>, "1e-46");
    REQUIRE_PARSE_FAIL(<double>, "-1e-46");

    REQUIRE_PARSE_FAIL(<double>, "1e-100");
    REQUIRE_PARSE_FAIL(<double>, "1e-308");
    REQUIRE_PARSE_FAIL(<double>, "-1e-308");
    REQUIRE_PARSE_FAIL(<double>, "-1e308");
    REQUIRE_PARSE_FAIL(<double>, "-1e309");
    REQUIRE_PARSE_FAIL(<double>, "1e-323");

    REQUIRE_PARSE_FAIL(<double>, "1e-324");
    REQUIRE_PARSE_FAIL(<double>, "1e65535");
    REQUIRE_PARSE_FAIL(<double>, "1e-65535");

    REQUIRE_PARSE_FAIL(<double>, "1e308");
    REQUIRE_PARSE_FAIL(<double>, "1e309");
  }
}
