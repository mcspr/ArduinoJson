// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Configuration.hpp>
#include <ArduinoJson/Numbers/FloatTraits.hpp>
#include <ArduinoJson/Numbers/JsonFloat.hpp>
#include <ArduinoJson/Polyfills/assert.hpp>
#include <ArduinoJson/Polyfills/math.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

struct FloatParts {
  uint32_t mantissa;
  int16_t exponent;
  int8_t pointIndex;
};

constexpr uint32_t pow10(int exponent) {
  return (exponent == 0) ? 1 : 10 * pow10(exponent - 1);
}

inline FloatParts decomposeFloat(JsonFloat value, int8_t significantDigits) {
  ARDUINOJSON_ASSERT(value > 0);
  ARDUINOJSON_ASSERT(significantDigits > 1);
  ARDUINOJSON_ASSERT(significantDigits <= 9);  // to prevent uint32_t overflow

  using traits = FloatTraits<JsonFloat>;

  bool useScientificNotation =
      value >= ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD ||
      value <= ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD;

  int16_t exponent = 0;
  int8_t index = traits::binaryPowersOfTenArraySize - 1;
  int bit = 1 << index;

  // Normalize value to range [1..10) and compute exponent
  if (value > 1) {
    for (; index >= 0; index--) {
      if (value >= traits::positiveBinaryPowersOfTen()[index]) {
        value *= traits::negativeBinaryPowersOfTen()[index];
        exponent = int16_t(exponent + bit);
      }
      bit >>= 1;
    }
  }
  ARDUINOJSON_ASSERT(value < 10);
  if (value < 1) {
    for (; index >= 0; index--) {
      if (value < traits::negativeBinaryPowersOfTen()[index] * 10) {
        value *= traits::positiveBinaryPowersOfTen()[index];
        exponent = int16_t(exponent - bit);
      }
      bit >>= 1;
    }
  }
  ARDUINOJSON_ASSERT(value >= 1);
  // ARDUINOJSON_ASSERT(value < 10);

  value *= JsonFloat(pow10(significantDigits - 1));

  auto mantissa = uint32_t(value);
  ARDUINOJSON_ASSERT(mantissa > 0);

  // rounding
  auto remainder = value - JsonFloat(mantissa);
  if (remainder >= 0.5)
    mantissa++;

  auto pointIndex = int8_t(significantDigits - 1);

  if (!useScientificNotation) {
    pointIndex = int8_t(pointIndex - int8_t(exponent));
    exponent = 0;
  }

  // remove trailing zeros
  while (mantissa % 10 == 0 && (useScientificNotation || pointIndex > 0)) {
    mantissa /= 10;
    if (pointIndex > 0)
      pointIndex--;
    else
      exponent++;
  }

  return {mantissa, exponent, pointIndex};
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
