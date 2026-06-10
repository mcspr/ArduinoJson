#pragma once

#include "CharPointer.hpp"

namespace ArduinoJson {
namespace Internals {
namespace Strings {
namespace StdString {

template <typename TString>
struct Equals {
  static bool Operator(const TString& str, const char* expected) {
    return Strings::CharPointer::Equals::Operator(str.c_str(), str.length(), expected);
  }
};

template <typename TString>
struct Append {
  static void Operator(TString& str, const char* other) {
    str += other;
  }

  static void Operator(TString& str, char c) {
    str += c;
  }
};

template <typename TString>
struct Duplicate {
  static const char* Operator(const TString& str, JsonBuffer* buffer) {
    return Strings::CharPointer::Duplicate::Operator(str.c_str(), buffer, str.length());
  }
};

}
}
}
}
