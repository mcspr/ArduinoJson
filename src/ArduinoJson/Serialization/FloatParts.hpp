// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"
#include "../TypeTraits/FloatTraits.hpp"

namespace ArduinoJson {
namespace Internals {

struct FloatParts {
  using value_type = uint32_t;
  using exponent_type = int16_t;
  using decimal_places_type = int8_t;

  value_type integral;
  value_type decimal;
  exponent_type exponent;
  decimal_places_type decimalPlaces;

  template <typename TFloat>
  static constexpr decimal_places_type decimalPlacesForType() {
    return sizeof(TFloat) >= 8
      ? 9
      : 6;
  }

  template <typename TFloat>
  static constexpr value_type maxDecimalPartForType() {
    // not larger than...
    return sizeof(TFloat) >= 8
      ? 1000000000 // ... 10 digits
      : 1000000;   // ... 7 digits
  }

  template <typename TFloat>
  static FloatParts make(TFloat value) {
    auto maxDecimalPart = maxDecimalPartForType<TFloat>();
    auto decimalPlaces = decimalPlacesForType<TFloat>();

    const auto normalized = normalize(value);

    auto integral = value_type(normalized.value);
    // reduce number of decimal places by the number of integral places
    for (auto tmp = integral; tmp >= 10; tmp /= 10) {
      maxDecimalPart /= 10;
      decimalPlaces--;
    }

    // retrieve decimal part as a integral value, adjusting number of visible digits in the process
    TFloat remainder = (normalized.value - TFloat(integral)) * TFloat(maxDecimalPart);

    // floor the value before processing and manually ceil based on the float calc above
    // increments by 1 if the remainder >= 0.5 (imprecise, possibly changing the resulting float)
    auto decimal = value_type(remainder);
    remainder -= TFloat(decimal);
    decimal += value_type(remainder * 2);

    auto exponent = normalized.powersOf10;
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
    exponent_type powersOf10;
  };

  template <typename TFloat>
  static NormalizedValue<TFloat> normalize(TFloat value) {
    typedef FloatTraits<TFloat> traits;
    exponent_type powersOf10 = 0;

    int8_t index = traits::binaryPowersOfTen - 1;
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
        if (value < traits::negativeBinaryPowerOfTen(index) * 10) {
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
