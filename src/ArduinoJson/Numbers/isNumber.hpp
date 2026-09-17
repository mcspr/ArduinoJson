// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Strings/Strings.hpp"
#include "parseNumber.hpp"

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

inline bool isNumber(const char *s, size_t len) {
  const auto result = JsonNumberParser::parse(s, len);
  return result && result.ok();
}

inline bool isNumber(const char *s) {
  return isNumber(s, Strings::Length::Operator(s));
}

}  // namespace Internals
}  // namespace ArduinoJson
