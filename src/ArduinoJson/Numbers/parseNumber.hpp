// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Strings/Strings.hpp"

#include "../TypeTraits/Conditional.hpp"
#include "../TypeTraits/FloatTraits.hpp"

#include "../Polyfills/ctype.hpp"
#include "../Polyfills/math.hpp"

#include "../Data/JsonFloat.hpp"
#include "../Data/JsonInteger.hpp"
#include "../Data/JsonNumber.hpp"

#include "convertNumber.hpp"

#include <cstdint>

namespace ArduinoJson {
namespace Internals {

template <typename TSigned, typename TUnsigned, typename TFloat>
struct ParsedNumberResultImpl {
  using Number = NumberImpl<TSigned, TUnsigned, TFloat>;
  using ValueType = typename Number::ValueType;
  using Type = typename Number::Type;

  Number value;

  bool ok() const {
    return value.ok();
  }

  explicit operator bool() const {
    return ok();
  }

  template <typename T>
  ConvertResult<T> convertTo() const {
    return value.template convertTo<T>();
  }
};

using ParsedNumberResult =
    ParsedNumberResultImpl<Internals::JsonInteger,
                           Internals::JsonUnsignedInteger,
                           Internals::JsonFloat>;

template <typename T>
static ParsedNumberResult fixedResult(char sign, T value) {
  ParsedNumberResult out;
  out.value = (sign == '-') ? -value : value;
  return out;
}

template <typename TOutput, typename TFloat>
struct FitIntoFloatResultImpl;

template <typename TOutput>
struct FitIntoFloatResultImpl<TOutput, float> {
  template <typename TMantissa, typename TExponent>
  static ParsedNumberResultImpl<JsonInteger, JsonUnsignedInteger, float>
  Operator(char sign, TMantissa mantissa, TExponent exponent) {
    return TOutput::template Operator<float>(sign, mantissa, exponent);
  }
};

template <typename TOutput>
struct FitIntoFloatResultImpl<TOutput, double> {
  template <typename TMantissa, typename TExponent>
  static ParsedNumberResultImpl<JsonInteger, JsonUnsignedInteger, double>
  Operator(char sign, TMantissa mantissa, TExponent exponent) {
    auto out = TOutput::template Operator<float>(sign, mantissa, exponent);
    if (!out)
      out = TOutput::template Operator<double>(sign, mantissa, exponent);

    return out;
  }
};

template <typename T, typename TMantissa, typename TExponent>
static constexpr bool floatWithinRange(TMantissa mantissa, TExponent exponent) {
  return (exponent >= FloatTraits<T>::exponent_min) &&
         (exponent <= FloatTraits<T>::exponent_max) &&
         (mantissa <= FloatTraits<T>::mantissa_max);
}

template <typename TFloat, typename TExponent>
static TFloat makeFloat(TFloat m, TExponent e) {
  using float_traits = FloatTraits<TFloat>;
  auto* const powersOfTen =
    e > 0
      ? &float_traits::positiveBinaryPowerOfTen
      : &float_traits::negativeBinaryPowerOfTen;

  if (e <= 0)
    e = TExponent(-e);

  for (size_t index = 0; e != 0; index++) {
    if (index >= float_traits::binaryPowersOfTen)
      return float_traits::nan();
    if (e & 1)
      m *= powersOfTen(index);
    e = static_cast<TExponent>(e >> 1);
  }

  return m;
}

struct FloatResultWithinRangeImpl {
  template <typename T, typename TMantissa, typename TExponent>
  static ParsedNumberResult
  Operator(char sign, TMantissa mantissa, TExponent exponent) {
    ParsedNumberResult out;

    if (floatWithinRange<T>(mantissa, exponent)) {
      const auto mantissa_within_range = static_cast<T>(mantissa);
      const auto value = makeFloat(mantissa_within_range, exponent);
      out.value = (sign == '-') ? -value : value;
    }

    return out;
  }
};

using FloatResultWithinRange =
    FitIntoFloatResultImpl<FloatResultWithinRangeImpl, JsonFloat>;

struct JsonNumberParser {
  template <typename A, typename B>
  using LargestType = Conditional<(sizeof(A) > sizeof(B)), A, B>;

  static ParsedNumberResult parse(const char* s, size_t len) {
    using float_traits = FloatTraits<JsonFloat>;
    using mantissa_type = LargestType<typename float_traits::mantissa_type, JsonUnsignedInteger>;
    using exponent_type = typename float_traits::exponent_type;

    ParsedNumberResult out;
    if (!s || !len)
      return out;

    const char* end = s + len;
    const char* it = s;

    char result_sign = '\0';
    char c = Strings::Copy::Operator(it);
    switch (c) {
      case '-':
      case '+':
        result_sign = c;
        ++it;
        break;
    }

    if (it == end)
      return out;

    c = Strings::Copy::Operator(it);

    // inf, Inf, infinity, Infinity
    if (c == 'i' || c == 'I') {
      const auto remaining = end - it;
      if (!((c == 'i' && remaining == 3) ||
            (c == 'I' && remaining == 8)))
        return out;

      const uint8_t expected[] = {'n', 'f', 'i', 'n', 'i', 't', 'y'};
      const auto common = Min(sizeof(expected), static_cast<size_t>(remaining - 1));
      if (Strings::Equals::Operator(&expected[0], common, it + 1, common))
        out = fixedResult(result_sign, float_traits::inf());

      return out;
    }

    // NaN, nan
    if (c == 'n' || c == 'N') {
      if (((it + 3) == end) &&
          (Strings::Copy::Operator(it + 1) == 'a') &&
          (Strings::Copy::Operator(it + 2) == c))
      {
        // no-op sign value, being lenient on any weird raw data
        out.value = float_traits::nan();
      }

      return out;
    }

    // only accept integral part or start of decimal part next
    if (!isdigit(c) && (c != '.'))
      return out;

    mantissa_type mantissa = 0;
    exponent_type exponent_offset = 0;

    uint8_t digit = 0xff;
    constexpr mantissa_type maxUint = JsonUnsignedInteger(-1);
    while (it != end) {
      c = Strings::Copy::Operator(it);
      if (!isdigit(c))
        break;

      digit = static_cast<uint8_t>(c - '0');
      if (mantissa > maxUint / 10)
        break;
      mantissa *= 10;
      if (mantissa > maxUint - digit)
        break;
      mantissa += digit;
      ++it;
    }

    if (it == end) {
      if (result_sign == '-') {
        constexpr mantissa_type sintMantissaMax =
            mantissa_type(1) << (sizeof(JsonInteger) * 8 - 1);
        if (mantissa <= sintMantissaMax) {
          out.value = JsonNumber(JsonInteger(~mantissa + 1));
        }

      } else {
        out.value = JsonNumber(JsonUnsignedInteger(mantissa));
      }

      if (out)
        return out;
    }

    // avoid mantissa overflow by offseting it w/ an exponent value
    constexpr exponent_type sintExponentMax =
        -float_traits::exponent_min + float_traits::exponent_max;

    while (mantissa > float_traits::mantissa_max) {
      mantissa /= 10;
      if ((exponent_offset + 1) > sintExponentMax)
        return out;

      exponent_offset++;
    }

    // ...since not every remaing digit can fit in the mantissa as-is
    // nb. while mantissa could be offset, exponent overflow is a failure state
    while (it != end) {
      c = Strings::Copy::Operator(it);
      if (!isdigit(c))
        break;

      if ((exponent_offset + 1) > sintExponentMax)
        return out;

      exponent_offset++;
      ++it;
    }

    if ((it != end) && Strings::Copy::Operator(it) == '.') {
      ++it;
  
      // both integral and decimal parts missing
      if ((it == end) && (digit == 0xff))
        return out;

      while (it != end) {
        c = Strings::Copy::Operator(it);
        if (!isdigit(c))
          break;

        // repeat the same overflow calc as its done for the integral part
        if (mantissa < float_traits::mantissa_max / 10) {
          mantissa *= 10;

          digit = static_cast<uint8_t>(c - '0');
          if (mantissa > float_traits::mantissa_max + digit)
            break;

          if ((exponent_offset - 1) < -sintExponentMax)
            return out;

          mantissa += digit;
          exponent_offset--;
        }

        ++it;
      }
    }

    exponent_type exponent = 0;
    char exponent_sign = '\0';

    if (it != end)
      c = Strings::Copy::Operator(it);

    if ((it != end) && (c == 'e' || c == 'E')) {
      ++it;

      // exponent value missing after specifier
      if (it == end)
        return out;

      c = Strings::Copy::Operator(it);
      switch (c) {
        case '-':
        case '+':
          exponent_sign = c;
          ++it;

          // exponent value missing after sign
          if (it == end)
            return out;

          break;
      }

      // the rest of exponent digits
      while (it != end) {
        c = Strings::Copy::Operator(it);
        if (!isdigit(c))
          break;

        digit = static_cast<uint8_t>(c - '0');
        if (exponent > (sintExponentMax / 10))
          return out;

        exponent = static_cast<exponent_type>(exponent * 10);
        if (exponent > (sintExponentMax - digit))
          return out;

        exponent = static_cast<exponent_type>(
          exponent + static_cast<exponent_type>(digit));
        ++it;
      }

      if (exponent_sign == '-')
        exponent = static_cast<exponent_type>(-exponent);
    }

    // 5.x and 7.x mimic js behaviour and return *something*
    // - negative exponent overflow returned 0.0
    // - positive exponent overflow returned inf
    // prefer a failure state instead, since we do return result instead of val by itself
    {
      exponent_type exponent_shift = sintExponentMax;
      exponent_shift = static_cast<exponent_type>(exponent_shift - Abs(exponent_offset));
      exponent_shift = static_cast<exponent_type>(exponent_shift - Abs(exponent));
      if (exponent_shift < 0)
        return out;
    }

    exponent = static_cast<exponent_type>(exponent + exponent_offset);

    // we should be at the end of the string, otherwise it's an error
    if (it == end) {
      // output defaults to smallest floating point type that could contain the value
      out = FloatResultWithinRange::Operator(result_sign, mantissa, exponent);
    }

    return out;
  }
};

// shortcuts for parser invocation w/ the current configuration options

inline ParsedNumberResult parseJsonNumber(const char* s, size_t len) {
  return JsonNumberParser::parse(s, len);
}

inline ParsedNumberResult parseJsonNumber(const char* s) {
  return parseJsonNumber(s, Strings::Length::Operator(s));
}

template <typename T>
inline ConvertResult<T> parseNumber(const char* s, size_t len) {
  const auto result = JsonNumberParser::parse(s, len);
  if (!result)
    return ConvertResult<T>();

  return result.template convertTo<T>();
}

template <typename T>
inline ConvertResult<T> parseNumber(const char* s) {
  return parseNumber<T>(s, Strings::Length::Operator(s));
}

}  // namespace Internals
}  // namespace ArduinoJson
