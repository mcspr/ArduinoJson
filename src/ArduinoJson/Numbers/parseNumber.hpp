// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#include "../TypeTraits/FloatTraits.hpp"
#include "../TypeTraits/Conditional.hpp"

#include "../Data/JsonInteger.hpp"
#include "../Data/JsonFloat.hpp"
#include "../Polyfills/ctype.hpp"

#include "convertNumber.hpp"

#include <cstdint>

namespace ArduinoJson {
namespace Internals {

template <typename A, typename B>
using LargestType = ConditionalType<(sizeof(A) > sizeof(B)), A, B>;

enum class NumberType : uint8_t {
  Invalid,
  Float,
  SignedInteger,
  UnsignedInteger,
#if ARDUINOJSON_USE_DOUBLE
  Double,
#endif
};

union NumberValue {
  NumberValue() {}
  NumberValue(float x) : asFloat(x) {}
  NumberValue(JsonInteger x) : asSignedInteger(x) {}
  NumberValue(JsonUInt x) : asUnsignedInteger(x) {}
#if ARDUINOJSON_USE_DOUBLE
  NumberValue(double x) : asDouble(x) {}
#endif

  JsonInteger asSignedInteger;
  JsonUInt asUnsignedInteger;
  float asFloat;
#if ARDUINOJSON_USE_DOUBLE
  double asDouble;
#endif
};

class Number {
  NumberType _type;
  NumberValue _value;

 public:
  Number() :
    _type(NumberType::Invalid)
  {}

  Number(float value) :
    _type(NumberType::Float),
    _value(value)
  {}

  Number(JsonInteger value) :
    _type(NumberType::SignedInteger),
    _value(value)
  {}

  Number(JsonUInt value) :
    _type(NumberType::UnsignedInteger),
    _value(value)
  {}

#if ARDUINOJSON_USE_DOUBLE
  Number(double value) :
    _type(NumberType::Double),
    _value(value)
  {}
#endif

  template <typename T>
  ConvertResult<T> convertTo() const {
    switch (_type) {
      case NumberType::Float:
        return convertNumber<T>(_value.asFloat);
      case NumberType::SignedInteger:
        return convertNumber<T>(_value.asSignedInteger);
      case NumberType::UnsignedInteger:
        return convertNumber<T>(_value.asUnsignedInteger);
#if ARDUINOJSON_USE_DOUBLE
      case NumberType::Double:
        return convertNumber<T>(_value.asDouble);
#endif
      default:
        break;
    }

    return ConvertResult<T>{};
  }

  NumberType type() const {
    return _type;
  }

  JsonInteger asSignedInteger() const {
    return _value.asSignedInteger;
  }

  JsonUInt asUnsignedInteger() const {
    return _value.asUnsignedInteger;
  }

  float asFloat() const {
    return _value.asFloat;
  }

#if ARDUINOJSON_USE_DOUBLE
  double asDouble() const {
    return _value.asDouble;
  }
#endif
};

struct ParsedNumberResult {
  Number value;
  
  bool ok() const {
    return value.type() != NumberType::Invalid;
  }

  explicit operator bool() const {
    return ok();
  }
  
  template <typename T>
  ConvertResult<T> convertTo() const {
    return value.convertTo<T>();
  }
};

template <typename T, typename TMantissa, typename TExponent>
static constexpr bool floatWithinRange(TMantissa mantissa, TExponent exponent) {
   return (exponent >= FloatTraits<T>::exponent_min) &&
          (exponent <= FloatTraits<T>::exponent_max) &&
          (mantissa <= FloatTraits<T>::mantissa_max);
}

struct JsonNumberParser {

  template <typename T>
  static ParsedNumberResult fixedResult(char sign, T value) {
    ParsedNumberResult out;
    out.value = (sign == '-') ? -value : value;
    return out;
  }

  template <typename T, typename TMantissa, typename TExponent>
  static ParsedNumberResult floatResultWithinRange(
        char sign, TMantissa mantissa, TExponent exponent)
  {
    ParsedNumberResult out;

    typedef FloatTraits<T> traits;
    if (floatWithinRange<T>(mantissa, exponent)) {
      const auto value = traits::make_float(T(mantissa), exponent);
      out.value = (sign == '-') ? -value : value;
    }

    return out;
  }

  static ParsedNumberResult parse(const char* s, size_t len) {
    typedef FloatTraits<JsonFloat> traits;
    typedef LargestType<typename traits::mantissa_type, JsonUInt> mantissa_t;
    typedef typename traits::exponent_type exponent_t;

    ParsedNumberResult out;
    if (!s || !len)
      return out;

    const char* end = s + len;
    const char* it = s;

    char result_sign = '\0';
    switch (*it) {
      case '-':
      case '+':
        result_sign = *it;
        ++it;
        break;
    }

    if (it == end)
      return out;

    // inf, Inf, infinity, Infinity
    if (*it == 'i' || *it == 'I') {
      auto remaining = end - it;
      switch (remaining) {
        case 3:
          if ((*it) != 'i')
            return out;
          break;

        case 8:
          if ((*it) != 'I')
            return out;
          break;
        
        default:
          return out;
      }

      const uint8_t expected[] = {'n', 'f', 'i', 'n', 'i', 't', 'y' };
      if (std::memcmp(&expected, it + 1, static_cast<size_t>(remaining - 1)) != 0)
        return out;

      out = fixedResult(result_sign, traits::inf());
      return out;
    }

    // NaN, nan
    if (*it == 'n' || *it == 'N') {
      if ((end - it) != 3)
        return out;

      ++it;
      if (*it != 'a')
        return out;

      ++it;
      if (*it != 'n' && *it != 'N')
        return out;

      // no-op sign value, being lenient on any weird raw data
      out.value = traits::nan();
      return out;
    }

    mantissa_t mantissa = 0;
    exponent_t exponent_offset = 0;
    const mantissa_t maxUint = JsonUInt(-1);

    while ((it != end) && isdigit(*it)) {
      uint8_t digit = uint8_t(*it - '0');
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
        const mantissa_t sintMantissaMax = mantissa_t(1)
                                           << (sizeof(JsonInteger) * 8 - 1);
        if (mantissa <= sintMantissaMax) {
          out.value = Number(JsonInteger(~mantissa + 1));
        }
      } else {
        out.value = Number(JsonUInt(mantissa));
      }

      if (out)
        return out;
    }

    // avoid mantissa overflow
    while (mantissa > traits::mantissa_max) {
      mantissa /= 10;
      exponent_offset++;
    }

    // remaing digits can't fit in the mantissa
    while ((it != end) && isdigit(*it)) {
      exponent_offset++;
      ++it;
    }

    if ((it != end) && *it == '.') {
      ++it;
      while ((it != end) && isdigit(*it)) {
        if (mantissa < traits::mantissa_max / 10) {
          mantissa = mantissa * 10 + uint8_t(*it - '0');
          exponent_offset--;
        }
        ++it;
      }
    }

    int exponent = 0;
    char exponent_sign = '\0';

    if ((it != end) && (*it == 'e' || *it == 'E')) {
      ++it;

      // exponent value missing after specifier
      if (it == end)
        return out;
      
      switch (*it) {
      case '-':
      case '+':
        exponent_sign = *it;
        ++it;

        // exponent value missing after sign
        if (it == end)
          return out;

        break;
      }

      // the rest of exponent digits
      while (it != end) {
        if (!isdigit(*it))
          break;

        exponent = exponent * 10 + (*it - '0');

        // 5.x and 7.x attepted to mimic js behaviour and return *something*
        // - negative exponent overflow would've returned 0.0
        // - positive exponent overflow would've returned inf
        // prefer a failure state instead, since we do return result instead of val by itself
        if (exponent_sign == '-') {
          if ((exponent + exponent_offset) > -traits::exponent_min)
            return out;
        } else if ((exponent + exponent_offset) > traits::exponent_max)
          return out;

        ++it;
      }

      if (exponent_sign == '-')
        exponent = -exponent;
    }

    exponent += exponent_offset;

    // we should be at the end of the string, otherwise it's an error
    if (it != end) {
      out = ParsedNumberResult();
      return out;
    }

    // probe for possible output types before returning
    out = floatResultWithinRange<float>(result_sign, mantissa, exponent);
#if ARDUINOJSON_USE_DOUBLE
    if (!out) {
        out = floatResultWithinRange<double>(result_sign, mantissa, exponent);
    }
#endif

    // couldn't fit into any type specified above
    return out;
  }
};

}  // namespace Internals
}  // namespace ArduinoJson
