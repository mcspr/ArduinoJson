// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#if ARDUINOJSON_ENABLE_PROGMEM

#include "../Strings/FlashString.hpp"
#include "../Readers/FlashString.hpp"

#include "StringTraitsBase.hpp"

class __FlashStringHelper;

namespace ArduinoJson {
namespace Internals {

template <>
struct StringTraitsImpl<const __FlashStringHelper*, void> : StringTraitsTag {
  typedef Readers::FlashString::Reader Reader;

  typedef Strings::FlashString::Copy Copy;
#if !ARDUINOJSON_CHAR_POINTER_IS_PROGMEM
  typedef Strings::FlashString::Duplicate Duplicate;
#endif
  typedef Strings::FlashString::Equals Equals;
  typedef Strings::FlashString::Length Length;
  typedef Strings::FlashString::Reference Reference;
};

}  // namespace Internals
}  // namespace ArduinoJson

#endif
