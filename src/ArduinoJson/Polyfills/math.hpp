// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include <cstdint>
#include <cstring>

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

inline bool SignBit(float x) {
  uint32_t tmp;
  std::memcpy(&tmp, &x, sizeof(x));
  return (tmp & uint32_t{0x80000000}) != 0;
}

inline bool SignBit(double x) {
  uint64_t tmp;
  std::memcpy(&tmp, &x, sizeof(x));
  return (tmp & uint64_t{0x8000000000000000}) != 0;
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
