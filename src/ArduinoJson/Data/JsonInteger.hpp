// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

namespace ArduinoJson {
namespace Internals {

using JsonInteger =
#if ARDUINOJSON_USE_LONG_LONG
  long long
#else
  long
#endif
  ;

using JsonUnsignedInteger =
#if ARDUINOJSON_USE_LONG_LONG
  unsigned long long
#else
  unsigned long
#endif
  ;

}  // namespace Internals
}  // namespace ArduinoJson
