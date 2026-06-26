// MIT License

#pragma once

#include "Strings.hpp"

#include "../TypeTraits/Constant.hpp"
#include "../TypeTraits/VoidType.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/Declval.hpp"

namespace ArduinoJson {
namespace Internals {
namespace Strings {
namespace StdString {
namespace Detail {

// TODO StringTraits used to bind this somewhere, and that is usually forcing some specializations
// no particular reason to begin checking exact type though, deduce things based on method availability

template <typename T, typename = void>
struct HasAppend : FalseType {
};

template <typename T>
struct HasAppend<T, VoidType<
    decltype(Declval<T>().append(Declval<const char*>(), Declval<size_t>())),
    decltype(Declval<T>().append(Declval<size_t>(), Declval<char>()))>>
  : TrueType {
};

template <typename T, typename = void>
struct HasConcat : FalseType {
};

template <typename T>
struct HasConcat<T, VoidType<
  decltype(Declval<T>().concat(Declval<const char*>(), Declval<size_t>())),
  decltype(Declval<T>().concat(Declval<char>()))>> : TrueType {
};

}

template <typename TString>
struct Equals {
  static bool Operator(const TString& str, const char* expected, size_t len) {
    return Strings::Equals::Operator(str.c_str(), str.length(), expected, len);
  }

  static bool Operator(const TString& str, const char* expected) {
    return Strings::Equals::Operator(str.c_str(), str.length(), expected);
  }
};

template <typename TString, typename = void>
struct Append {
  static void Operator(TString& str, const char* other) {
    str += other;
  }

  static void Operator(TString& str, char c) {
    str += c;
  }
};

template <typename TString>
struct Append<TString,
  typename EnableIf<Detail::HasAppend<TString>::value>::type> {

  static void Operator(TString& str, const char* other, size_t len) {
#if ARDUINOJSON_CHAR_POINTER_IS_PROGMEM
    if (Strings::FlashString::Probe(other)) {
      str.resize(len);
      Strings::FlashString::Copy::Operator(const_cast<char *>(str.data()), other, len);
    } else
#endif
      str.append(other, len);
  }

  static void Operator(TString& str, const char* other) {
    Operator(str, other, Strings::Length::Operator(other));
  }

  static void Operator(TString& str, char c) {
    str.append(1, c);
  }
};

template <typename TString>
struct Append<TString,
  typename EnableIf<Detail::HasConcat<TString>::value>::type> {

  static void Operator(TString& str, const char* other) {
    str.concat(other);
  }

  static void Operator(TString& str, const char* other, size_t len) {
    str.concat(other, len);
  }

  static void Operator(TString& str, char c) {
    str.concat(&c, 1);
  }
};

template <typename TString>
struct Duplicate {
  static const char* Operator(JsonBuffer* buffer, const TString& str) {
    return Strings::CharPointer::Duplicate::Operator(buffer, str.c_str(), str.length());
  }
};

}
}
}
}
