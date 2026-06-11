// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#if ARDUINOJSON_ENABLE_PROGMEM

#include "../Readers/FlashString.hpp"
#include "../Strings/FlashString.hpp"
#include "../Strings/CharPointer.hpp"

#include "StringTraitsBase.hpp"

class __FlashStringHelper;

namespace ArduinoJson {
namespace Internals {

template <>
struct StringTraitsImpl<const __FlashStringHelper*, void> : StringTraitsTag {
  typedef Readers::FlashString::Reader Reader;
  typedef Strings::FlashString::Equals Equals;
#if !ARDUINOJSON_CHAR_POINTER_IS_PROGMEM
  typedef Strings::FlashString::Duplicate Duplicate;
#endif
};

}  // namespace Internals
}  // namespace ArduinoJson

#endif
