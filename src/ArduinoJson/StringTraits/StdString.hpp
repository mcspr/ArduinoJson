// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#include "../Readers/StdString.hpp"
#include "../Strings/StdString.hpp"

#include "StringTraitsBase.hpp"

#if ARDUINOJSON_ENABLE_STD_STRING || ARDUINOJSON_ENABLE_ARDUINO_STRING

#if ARDUINOJSON_ENABLE_ARDUINO_STRING
#include <WString.h>
#endif

#if ARDUINOJSON_ENABLE_STD_STRING
#include <string>
#endif

namespace ArduinoJson {
namespace Internals {

template <typename TString>
struct StdStringTraits : StringTraitsTag {
  typedef Readers::StdString::Reader<TString> Reader;
  typedef Strings::StdString::Length<TString> Length;
  typedef Strings::StdString::Equals<TString> Equals;
  typedef Strings::StdString::Append<TString> Append;
  typedef Strings::StdString::Duplicate<TString> Duplicate;
};

#if ARDUINOJSON_ENABLE_ARDUINO_STRING
template <>
struct StringTraitsImpl<String>
  : StdStringTraits<String> {
};

template <>
struct StringTraitsImpl<StringSumHelper>
  : StdStringTraits<StringSumHelper> {
};
#endif

#if ARDUINOJSON_ENABLE_STD_STRING
template <>
struct StringTraitsImpl<std::string>
  : StdStringTraits<std::string> {
};
#endif
}  // namespace Internals
}  // namespace ArduinoJson

#endif
