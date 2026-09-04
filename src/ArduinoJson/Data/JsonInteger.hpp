// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#include <cstdint>

namespace ArduinoJson {
namespace Internals {

using JsonInteger =
#if ARDUINOJSON_USE_LONG_LONG
  int64_t
#else
  int32_t
#endif
  ;

using JsonUnsignedInteger =
#if ARDUINOJSON_USE_LONG_LONG
  uint64_t
#else
  uint32_t
#endif
  ;

}  // namespace Internals
}  // namespace ArduinoJson
