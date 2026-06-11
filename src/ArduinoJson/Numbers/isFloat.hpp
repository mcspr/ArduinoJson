// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"
#include "../Strings/Strings.hpp"

#include "parseNumber.hpp"

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

inline bool isFloat(const char *s, size_t len) {
  const auto result = JsonNumberParser::parse(s, len);
  if (result) {
    switch (result.value.type()) {
    case NumberType::SignedInteger:
    case NumberType::UnsignedInteger:
    case NumberType::Float:
#if ARDUINOJSON_USE_DOUBLE
    case NumberType::Double:
#endif
      return true;

    default:
      break;
    }
  }

  return false;
}

inline bool isFloat(const char *s) {
  return isFloat(s, Strings::Length::Operator(s));
}

}  // namespace Internals
}  // namespace ArduinoJson
