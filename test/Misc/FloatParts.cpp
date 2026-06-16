// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson/Serialization/FloatParts.hpp>

#include <catch.hpp>
#include <limits>

// note that current behaviour is close'ish to printf(3) %.10e, causing the
// last decimal digit to increment by 1 whenever the digit after it is >=5
// sometimes this would also cause roundtrip errors, producing invalid floats
using ArduinoJson::Internals::FloatParts;

TEST_CASE("FloatParts<double>") {
  SECTION("std::numeric_limits<double>::max()") {
    constexpr auto maxval = std::numeric_limits<double>::max(); // 1.7976931348623157E+308
    const auto parts = FloatParts::make<double>(maxval);
    REQUIRE(parts.integral == 1);
    REQUIRE(parts.decimal == 797693135); // rounded up by the serializer
    REQUIRE(parts.decimalPlaces == 9);
    REQUIRE(parts.exponent == 308);
    // constexpr auto invalid = 1.797693135e308;
  }

  SECTION("std::numeric_limits<double>::denorm_min()") {
    constexpr auto minval = std::numeric_limits<double>::denorm_min(); // 4.94065645841247e-324
    const auto parts = FloatParts::make<double>(minval);
    REQUIRE(parts.integral == 4);
    REQUIRE(parts.decimal == 940656458);
    REQUIRE(parts.decimalPlaces == 9);
    REQUIRE(parts.exponent == -324);
    // constexpr auto valid = 4.940656458e-324;
  }

  SECTION("std::numeric_limits<double>::min()") {
    constexpr auto minval = std::numeric_limits<double>::min(); // 2.2250738585072014e-308
    const auto parts = FloatParts::make<double>(minval);
    REQUIRE(parts.integral == 2);
    REQUIRE(parts.decimal == 225073859); // rounded up by the serializer
    REQUIRE(parts.decimalPlaces == 9);
    REQUIRE(parts.exponent == -308);
    // constexpr auto valid = 2.225073859e-308;
  }
}

TEST_CASE("FloatParts<float>") {
  SECTION("std::numeric_limits<float>::max()") {
    constexpr auto maxval = std::numeric_limits<float>::max(); // 3.40282347E+38
    const auto parts = FloatParts::make<float>(maxval);
    REQUIRE(parts.integral == 3);
    REQUIRE(parts.decimal == 402823);
    REQUIRE(parts.decimalPlaces == 6);
    REQUIRE(parts.exponent == 38);
    // constexpr auto valid = 3.402823e-38f;
  }

  SECTION("std::numeric_limits<float>::denorm_min()") {
    constexpr auto minval = std::numeric_limits<float>::denorm_min(); // 1.40129846e-45
    const auto parts = FloatParts::make<float>(minval);
    REQUIRE(parts.integral == 1);
    REQUIRE(parts.decimal == 401299); // rounded up by the serializer
    REQUIRE(parts.decimalPlaces == 6);
    REQUIRE(parts.exponent == -45);
    // constexpr auto valid = 1.401299e-45f;
  }

  SECTION("std::numeric_limits<float>::min()") {
    constexpr auto minval = std::numeric_limits<float>::min(); // 1.17549435e−38
    const auto parts = FloatParts::make<float>(minval);
    REQUIRE(parts.integral == 1);
    REQUIRE(parts.decimal == 175494);
    REQUIRE(parts.decimalPlaces == 6);
    REQUIRE(parts.exponent == -38);
    // constexpr auto valid = 1.175494e-38;
  }
}
