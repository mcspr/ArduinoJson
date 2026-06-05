// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include "../TypeTraits/FloatTraits.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsIntegral.hpp"
#include "../TypeTraits/IsFloatingPoint.hpp"
#include "../TypeTraits/IsSigned.hpp"
#include "../TypeTraits/IsUnsigned.hpp"

#include "../TypeTraits/Constant.hpp"
#include "../TypeTraits/And.hpp"

#include <limits> // 7.x includes polyfill with a slightly different naming b/c of min and max macros

namespace ArduinoJson {
namespace Internals {

// uint32 -> int32
// uint64 -> int32
template <typename TOut, typename TIn>
constexpr
typename EnableIf<
  And<IsIntegral<TIn>, IsUnsigned<TIn>,
      IsIntegral<TOut>, BooleanConstant<sizeof(TOut) <= sizeof(TIn)>>::value, bool>::type
canConvertNumber(TIn value) {
  return value <= TIn(std::numeric_limits<TOut>::max());
}

// uint32 -> int64
template <typename TOut, typename TIn>
constexpr
typename EnableIf<
  And<IsIntegral<TIn>, IsUnsigned<TIn>,
      IsIntegral<TOut>, BooleanConstant<sizeof(TIn) < sizeof(TOut)>>::value, bool>::type
canConvertNumber(TIn) {
  return true;
}

// uint32 -> float
// int32 -> float
template <typename TOut, typename TIn>
constexpr
typename EnableIf<
  And<IsIntegral<TIn>,
      IsFloatingPoint<TOut>>::value, bool>::type
canConvertNumber(TIn) {
  return true;
}

// int64 -> int32
template <typename TOut, typename TIn>
constexpr
typename EnableIf<
  And<IsIntegral<TIn>, IsSigned<TIn>,
      IsIntegral<TOut>, IsSigned<TOut>,
      BooleanConstant<sizeof(TOut) < sizeof(TIn)>>::value, bool>::type
canConvertNumber(TIn value) {
  return value >= TIn(std::numeric_limits<TOut>::min()) &&
         value <= TIn(std::numeric_limits<TOut>::max());
}

// int32 -> int32
// int32 -> int64
template <typename TOut, typename TIn>
constexpr
typename EnableIf<
  And<IsIntegral<TIn>, IsSigned<TIn>,
      IsIntegral<TOut>, IsSigned<TOut>,
      BooleanConstant<sizeof(TIn) <= sizeof(TOut)>>::value, bool>::type
canConvertNumber(TIn) {
  return true;
}

// int32 -> uint32
// int32 -> uint64
template <typename TOut, typename TIn>
constexpr
typename EnableIf<
  And<IsIntegral<TIn>, IsSigned<TIn>,
      IsIntegral<TOut>, IsUnsigned<TOut>,
      BooleanConstant<sizeof(TOut) >= sizeof(TIn)>>::value, bool>::type
canConvertNumber(TIn value) {
  return (value >= 0) && (TOut(value) <= std::numeric_limits<TOut>::max());
}

// int32 -> uint16
template <typename TOut, typename TIn>
constexpr
typename EnableIf<
  And<IsIntegral<TIn>, IsSigned<TIn>,
      IsIntegral<TOut>, IsUnsigned<TOut>,
      BooleanConstant<sizeof(TOut) < sizeof(TIn)>>::value, bool>::type
canConvertNumber(TIn value) {
  return (value >= 0) && (TIn(value) <= std::numeric_limits<TOut>::max());
}

// float32 -> int16
// float64 -> int32
template <typename TOut, typename TIn>
constexpr
typename EnableIf<
  And<IsFloatingPoint<TIn>,
      IsIntegral<TOut>,
      BooleanConstant<sizeof(TOut) < sizeof(TIn)>>::value, bool>::type
canConvertNumber(TIn value) {
  return value >= std::numeric_limits<TOut>::min() &&
         value <= std::numeric_limits<TOut>::max();
}

// float32 -> int32
// float32 -> uint32
// float32 -> int64
// float32 -> uint64
// float64 -> int64
// float64 -> uint64
template <typename TOut, typename TIn>
constexpr
typename EnableIf<
    And<IsFloatingPoint<TIn>,
        IsIntegral<TOut>,
        BooleanConstant<sizeof(TOut) >= sizeof(TIn)>>::value, bool>::type
canConvertNumber(TIn value) {
  // Avoid error "9.22337e+18 is outside the range of representable values of
  // type 'long'"
  return value >= std::numeric_limits<TOut>::min() &&
         value <= FloatTraits<TIn>::template highest_for<TOut>();
}

// float32 -> float32
// float64 -> float64
// float64 -> float32
template <typename TOut, typename TIn>
constexpr
typename EnableIf<
  And<IsFloatingPoint<TIn>,
      IsFloatingPoint<TOut>>::value, bool>::type
canConvertNumber(TIn) {
  return true;
}

template <typename T>
struct ConvertResult {
 private:
  bool _ok;

 public:
  T value;

  constexpr ConvertResult(T v) :
    _ok(true),
    value(v)
  {}

  constexpr ConvertResult() :
    _ok(false),
    value()
  {}

  constexpr bool ok() const {
    return _ok;
  }

  constexpr explicit operator bool() const {
    return ok();
  }
};

template <typename TOut, typename TIn, typename Result = ConvertResult<TOut>>
struct ConvertNumber {
  static constexpr Result convert(TIn value) {
    return canConvertNumber<TOut>(value)
      ? Result{ TOut(value) }
      : Result{};
  }
};

template <typename TOut, typename TIn>
constexpr ConvertResult<TOut> convertNumber(TIn value) {
  return ConvertNumber<TOut, TIn>::convert(value);
}

}
}
