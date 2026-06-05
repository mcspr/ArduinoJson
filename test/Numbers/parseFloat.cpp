// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson/Numbers/parseFloat.hpp>
#include <ArduinoJson/Polyfills/math.hpp>
#include <catch.hpp>

#include <cmath>

using ArduinoJson::Internals::JsonNumberParser;
using ArduinoJson::Internals::parseFloat;

using ArduinoJson::Internals::isNaN;
using ArduinoJson::Internals::isInfinity;

template <typename T>
void check(const char *input, size_t len, T expected) { 
  CAPTURE(input);
  const auto result = parseFloat<T>(&input[0], len);
  REQUIRE(result.ok());
  REQUIRE(result.value == Approx(expected));
}

template <typename T>
inline void check(const char *input, T expected) { 
  check(input, strlen(input), expected);
}

template <typename T>
void checkSigned(const char *input, size_t len, T expected, bool negative) {
  CAPTURE(input);
  const auto result = parseFloat<T>(&input[0], len);
  REQUIRE(result.ok());
  REQUIRE(std::signbit(result.value) == negative);
  REQUIRE(result.value == Approx(expected));
}

template <typename T>
inline void checkSigned(const char *input, T expected, bool negative) {
  checkSigned(input, strlen(input), expected, negative);
}

template <typename T>
void checkFail(const char *input, size_t len) {
  CAPTURE(input);
  const auto result = parseFloat<T>(&input[0], len);
  REQUIRE_FALSE(result.ok());
}

template <typename T>
inline void checkFail(const char *input) {
  checkFail<T>(input, strlen(input));
}

template <typename T>
void checkConvertFail(const char *input, size_t len) {
  CAPTURE(input);
  const auto parse = JsonNumberParser::parse(&input[0], len);
  REQUIRE(parse);
  const auto convert = parse.value.template convertTo<T>();
  REQUIRE_FALSE(convert);
}

template <typename T>
inline void checkConvertFail(const char *input) {
  checkConvertFail<T>(input, strlen(input));
}

template <typename T>
void checkNaN(const char *input, size_t len) {
  CAPTURE(input);
  //const auto result = parseFloat<T>(&input[0], len);
  const auto parse = JsonNumberParser::parse(&input[0], len);
  REQUIRE(parse.ok());
  const auto convert = parse.convertTo<T>();
  REQUIRE(convert.ok());
  REQUIRE(isNaN(convert.value));
  //REQUIRE(result);
  //REQUIRE(isNaN(result.value));
}

template <typename T>
inline void checkNaN(const char *input) {
  checkNaN<T>(input, strlen(input));
}

template <typename T>
void checkInf(const char* input, bool negative) {
  CAPTURE(input);
  const auto result = parseFloat<T>(input);
  REQUIRE(result.ok());

  REQUIRE(std::signbit(result.value) == negative);
  REQUIRE_FALSE(isNaN(result.value));
  REQUIRE(isInfinity(result.value));
}

TEST_CASE("parseFloat<float>()") {
  SECTION("Empty") {
    checkFail<float>("");
  }

  SECTION("Zero") {
    check<float>("0", 0.0f);
    check<float>("0.", 0.0f);
    checkSigned<float>("+0.", 0.0f, false);
    checkSigned<float>("-0.", 0.0f, true);
    check<float>("0.0", 0.0f);
    checkSigned<float>("+0.0", 0.0f, false);
    checkSigned<float>("-0.0", -0.0f, true);
  }

  SECTION("Float_Short_NoExponent") {
    check<float>("3.14", 3.14f);
    check<float>("-3.14", -3.14f);
    check<float>("+3.14", +3.14f);
  }

  SECTION("Short_NoDot") {
    check<float>("1E+38", 1E+38f);
    check<float>("-1E+38", -1E+38f);
    check<float>("+1E-38", +1E-38f);
    check<float>("+1e+38", +1e+38f);
    check<float>("-1e-38", -1e-38f);
  }

  SECTION("Max") {
    check<float>("340.2823e+36", 3.402823e+38f);
    check<float>("34.02823e+37", 3.402823e+38f);
    check<float>("3.402823e+38", 3.402823e+38f);
    check<float>("0.3402823e+39", 3.402823e+38f);
    check<float>("0.03402823e+40", 3.402823e+38f);
    check<float>("0.003402823e+41", 3.402823e+38f);
  }

  SECTION("VeryLong") {
    check<float>("0.00000000000000000000000000000001", 1e-32f);
    check<float>("100000000000000000000000000000000.0", 1e+32f);
    check<float>(
        "100000000000000000000000000000000.00000000000000000000000000000",
        1e+32f);
  }

  SECTION("MantissaTooLongToFit") {
    check<float>("0.340282346638528861111111111111", 0.34028234663852886f);
    check<float>("34028234663852886.11111111111111", 34028234663852886.0f);
    check<float>("34028234.66385288611111111111111", 34028234.663852886f);

    check<float>("-0.340282346638528861111111111111", -0.34028234663852886f);
    check<float>("-34028234663852886.11111111111111", -34028234663852886.0f);
    check<float>("-34028234.66385288611111111111111", -34028234.663852886f);
  }

  SECTION("ExponentTooBig") {
    checkInf<float>("1e39", false);
    check<float>("1e38", 1e38f);
    checkInf<float>("-1e39", true);
    check<float>("-1e38", -1e38f);
    check<float>("1e-45", 1e-45f);
    check<float>("1e-46", 0.0f);
    checkInf<float>("1e255", false);
    check<float>("1e-255", 0.0f);
  }

  SECTION("NaN") {
    checkNaN<float>("NaN");
    checkNaN<float>("nan");
  }

  SECTION("Infinity") {
    checkInf<float>("Infinity", false);
    checkInf<float>("+Infinity", false);
    checkInf<float>("-Infinity", true);
    checkInf<float>("inf", false);
    checkInf<float>("+inf", false);
    checkInf<float>("-inf", true);
  }

  SECTION("Boolean") {
    checkFail<float>("false");
    checkFail<float>("true");
  }

  SECTION("Overflow exponent with decimal part") {  // Issue #2220
    checkFail<float>(
        "0.000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000001");
  }

  SECTION("Overflow exponent with integral part") {
    checkFail<float>(
        "10000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000");
  }
}

TEST_CASE("parseFloat<double>()") {
  SECTION("Empty") {
    checkFail<double>("");
  }

  SECTION("Zero") {
    check<double>("0", 0.0);
    check<double>("0.", 0.0);
    checkSigned<double>("+0.", 0.0, false);
    checkSigned<double>("-0.", 0.0, true);
    check<double>("0.0", 0.0);
    checkSigned<double>("+0.0", 0.0, false);
    checkSigned<double>("-0.0", -0.0, true);
  }

  SECTION("Short_NoExponent") {
    check<double>("3.14", 3.14);
    check<double>("-3.14", -3.14);
    check<double>("+3.14", +3.14);
  }

  SECTION("Short_NoDot") {
    check<double>("1E+308", 1E+308);
    check<double>("-1E+308", -1E+308);
    check<double>("+1E-308", +1E-308);
    check<double>("+1e+308", +1e+308);
    check<double>("-1e-308", -1e-308);
  }

  SECTION("Max") {
    check<double>(".017976931348623147e+310", 1.7976931348623147e+308);
    check<double>(".17976931348623147e+309", 1.7976931348623147e+308);
    check<double>("1.7976931348623147e+308", 1.7976931348623147e+308);
    check<double>("17.976931348623147e+307", 1.7976931348623147e+308);
    check<double>("179.76931348623147e+306", 1.7976931348623147e+308);
  }

  SECTION("Min") {
    check<double>(".022250738585072014e-306", 2.2250738585072014e-308);
    check<double>(".22250738585072014e-307", 2.2250738585072014e-308);
    check<double>("2.2250738585072014e-308", 2.2250738585072014e-308);
    check<double>("22.250738585072014e-309", 2.2250738585072014e-308);
    check<double>("222.50738585072014e-310", 2.2250738585072014e-308);
  }

  SECTION("VeryLong") {
    check<double>("0.00000000000000000000000000000001", 1e-32);
    check<double>("100000000000000000000000000000000.0", 1e+32);
    check<double>(
        "100000000000000000000000000000000.00000000000000000000000000000",
        1e+32);
  }

  SECTION("MantissaTooLongToFit") {
    check<double>("0.179769313486231571111111111111", 0.17976931348623157);
    check<double>("17976931348623157.11111111111111", 17976931348623157.0);
    check<double>("1797693.134862315711111111111111", 1797693.1348623157);

    check<double>("-0.179769313486231571111111111111", -0.17976931348623157);
    check<double>("-17976931348623157.11111111111111", -17976931348623157.0);
    check<double>("-1797693.134862315711111111111111", -1797693.1348623157);
  }

  SECTION("ExponentTooBig") {
    check<double>("-1e308", -1e308);
    checkFail<double>("-1e309");
    check<double>("1e308", 1e308);
    checkFail<double>("1e309");
    check<double>("1e-323", 1e-323);
    checkFail<double>("1e-324");
    checkFail<double>("1e65535");
    checkFail<double>("1e-65535");
  }

  SECTION("NaN") {
    checkNaN<double>("NaN");
    checkNaN<double>("nan");
    checkNaN<double>("-NaN");
    checkNaN<double>("+NaN");
    checkNaN<double>("-nan");
    checkNaN<double>("+nan");
  }

  SECTION("Boolean") {
    checkFail<double>("false");
    checkFail<double>("true");
  }

  SECTION("Overflow exponent with decimal part") {  // Issue #2220
    checkFail<double>(
        "0.000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000001");
  }

  SECTION("Overflow exponent with integral part") {
    checkFail<double>(
        "10000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000"
        "00000000000000000000000000000000000000000000000000");
  }
}
