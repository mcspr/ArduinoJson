// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "convertNumber.hpp"
#include "parseNumber.hpp"

#include <cstring>

namespace ArduinoJson {
namespace Internals {

template <typename T>
inline ConvertResult<T> parseInteger(const char *s, size_t len) {
  auto result = JsonNumberParser::parse(s, len);
  if (!result)
    return ConvertResult<T>();

  return result.convertTo<T>();
}

template <typename T>
inline ConvertResult<T> parseInteger(const char *s) {
  return parseInteger<T>(s, strlen(s));
}

}  // namespace Internals
}  // namespace ArduinoJson
