// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"
#include "../TypeTraits/FloatTraits.hpp"

namespace ArduinoJson {
namespace Internals {

struct FloatParts {
  uint32_t integral;
  uint32_t decimal;
  int16_t exponent;
  int8_t decimalPlaces;

  template <typename TFloat>
  static FloatParts make(TFloat value) {
    uint32_t maxDecimalPart = sizeof(TFloat) >= 8 ? 1000000000 : 1000000;
    int8_t decimalPlaces = sizeof(TFloat) >= 8 ? 9 : 6;

    const auto normalized = normalize(value);

    uint32_t integral = uint32_t(normalized.value);
    // reduce number of decimal places by the number of integral places
    for (uint32_t tmp = integral; tmp >= 10; tmp /= 10) {
      maxDecimalPart /= 10;
      decimalPlaces--;
    }

    TFloat remainder = (normalized.value - TFloat(integral)) * TFloat(maxDecimalPart);

    uint32_t decimal = uint32_t(remainder);
    remainder = remainder - TFloat(decimal);

    // rounding:
    // increment by 1 if remainder >= 0.5
    int16_t exponent = normalized.powersOf10;
    decimal += uint32_t(remainder * 2);
    if (decimal >= maxDecimalPart) {
      decimal = 0;
      integral++;
      if (exponent && integral >= 10) {
        exponent++;
        integral = 1;
      }
    }

    // remove trailing zeros
    while (decimal % 10 == 0 && decimalPlaces > 0) {
      decimal /= 10;
      decimalPlaces--;
    }

    return {integral, decimal, exponent, decimalPlaces};
  }

 private:
  template <typename TFloat>
  struct NormalizedValue {
    TFloat value;
    int16_t powersOf10;
  };

  template <typename TFloat>
  static NormalizedValue<TFloat> normalize(TFloat value) {
    typedef FloatTraits<TFloat> traits;
    int16_t powersOf10 = 0;

    int8_t index = sizeof(TFloat) == 8 ? 8 : 5;
    int bit = 1 << index;

    if (value >= ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD) {
      for (; index >= 0; index--) {
        if (value >= traits::positiveBinaryPowerOfTen(index)) {
          value *= traits::negativeBinaryPowerOfTen(index);
          powersOf10 = int16_t(powersOf10 + bit);
        }
        bit >>= 1;
      }
    }

    if (value > 0 && value <= ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD) {
      for (; index >= 0; index--) {
        if (value < traits::negativeBinaryPowerOfTenPlusOne(index)) {
          value *= traits::positiveBinaryPowerOfTen(index);
          powersOf10 = int16_t(powersOf10 - bit);
        }
        bit >>= 1;
      }
    }

    return {value, powersOf10};
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
