// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include <cstring>

#if defined(__GNUC__)
#if __GNUC__ >= 5
#include <type_traits>
#endif
#endif

namespace ArduinoJson {
namespace Internals {

template <typename To, typename From>
To bit_cast(const From& raw_data) {
  static_assert(sizeof(To) == sizeof(From), "");

// gcc 4.8 incomplete -std=c++11
#if defined(__GNUC__)
#if __GNUC__ >= 5
  static_assert(std::is_trivially_copy_constructible<To>::value, "");
  static_assert(std::is_trivially_copyable<To>::value, "");
  static_assert(std::is_trivially_copyable<From>::value, "");
#endif
#endif

  To out;
  std::memcpy(&out, &raw_data, sizeof(To));
  return out;
}

}  // namespace Internals
}  // namespace ArduinoJson
