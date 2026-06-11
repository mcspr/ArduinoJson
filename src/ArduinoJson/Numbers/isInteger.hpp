// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Strings/Strings.hpp"

#include "parseNumber.hpp"

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

inline bool isInteger(const char *s, size_t len) {
  const auto result = JsonNumberParser::parse(s, len);
  if (result) {
    switch (result.value.type()) {
    case NumberType::SignedInteger:
    case NumberType::UnsignedInteger:
      return true;

    default:
      break;
    }
  }

  return false;
}

inline bool isInteger(const char *s) {
  return isInteger(s, Strings::Length::Operator(s));
}


}  // namespace Internals
}  // namespace ArduinoJson
