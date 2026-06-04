// MIT License

#pragma once

namespace ArduinoJson {
namespace Internals {

template <typename T, T Value>
struct IntegralConstant {
  typedef IntegralConstant<T, Value> type;
  static constexpr T value = Value;
};

template <bool Value>
using BooleanConstant = IntegralConstant<bool, Value>;

using FalseType = IntegralConstant<bool, false>;
using TrueType = IntegralConstant<bool, true>;

}  // namespace Internals
}  // namespace ArduinoJson
