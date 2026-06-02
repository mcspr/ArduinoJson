// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

namespace ArduinoJson {
namespace Internals {

inline bool isNaN(float x) {
  return x != x;
}

inline bool isNaN(double x) {
  return x != x;
}

inline bool isInfinity(float x) {
  return x != 0.0f && x * 2.0f == x;
}

inline bool isInfinity(double x) {
  return x != 0.0 && x * 2.0 == x;
}

template <typename T>
inline constexpr const T& Min(const T& lhs, const T& rhs) {
  return rhs < lhs ? rhs : lhs;
}

template <typename T>
inline constexpr const T& Max(const T& lhs, const T& rhs) {
  return  lhs < rhs ? rhs : lhs;
}

}  // namespace Internals
}  // namespace ArduinoJson
