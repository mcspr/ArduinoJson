// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#include "StringTraitsBase.hpp"
#include "CharPointer.hpp"

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
struct StdStringTraits {
  typedef const char* duplicate_type;
  typedef CharPointerTraits<char> traits_type;

  template <typename Buffer>
  static duplicate_type duplicate(const TString& str, Buffer* buffer) {
    return traits_type::duplicate(str.c_str(), buffer, str.length());
  }

  static bool is_null(const TString& str) {
    // Arduino's String::c_str() can return NULL
    return !str.c_str();
  }

  struct Reader : CharPointerTraits<char>::Reader {
    Reader(const TString& str) :
      traits_type::Reader(str.c_str(), str.length())
    {}
  };

  static bool equals(const TString& str, const char* expected) {
    return traits_type::equals(str.c_str(), str.length(), expected);
  }

  static void append(TString& str, const char* s) {
    str += s;
  }

  static void append(TString& str, char c) {
    str += c;
  }

  typedef TrueType has_append;
  typedef TrueType has_equals;
  typedef TrueType should_duplicate;
};

#if ARDUINOJSON_ENABLE_ARDUINO_STRING
template <>
struct StringTraitsImpl<String, void> : StdStringTraits<String> {};
template <>
struct StringTraitsImpl<const String, void> : StdStringTraits<String> {};

template <>
struct StringTraitsImpl<StringSumHelper, void> : StdStringTraits<StringSumHelper> {
};
template <>
struct StringTraitsImpl<const StringSumHelper, void> : StdStringTraits<StringSumHelper> {
};
#endif

#if ARDUINOJSON_ENABLE_STD_STRING
template <>
struct StringTraitsImpl<std::string, void> : StdStringTraits<std::string> {};

template <>
struct StringTraitsImpl<const std::string, void> : StringTraitsImpl<std::string, void> {};
#endif
}  // namespace Internals
}  // namespace ArduinoJson

#endif
