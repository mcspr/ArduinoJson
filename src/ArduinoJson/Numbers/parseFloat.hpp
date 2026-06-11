// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Strings/Strings.hpp"

#include "convertNumber.hpp"
#include "parseNumber.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename T>
inline ConvertResult<T> parseFloat(const char *s, size_t len) {
  const auto result = JsonNumberParser::parse(s, len);
  if (!result)
    return ConvertResult<T>();

  return result.convertTo<T>();
}

template <typename T>
inline ConvertResult<T> parseFloat(const char *s) {
  return parseFloat<T>(s, Strings::Length::Operator(s));
}

}  // namespace Internals
}  // namespace ArduinoJson
