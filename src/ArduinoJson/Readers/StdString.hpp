// MIT License

#pragma once

#include "../Configuration.hpp"

#include "Base.hpp"
#include "CharPointer.hpp"

#if ARDUINOJSON_ENABLE_ARDUINO_STRING
#include <WString.h>
#endif

#if ARDUINOJSON_ENABLE_STD_STRING
#include <string>
#endif

namespace ArduinoJson {
namespace Internals {
namespace Readers {
namespace StdString {

template <typename T>
struct Reader : Readers::CharPointer::Reader {
  explicit Reader(T& str) :
    CharPointer::Reader(str.c_str(), str.length())
  {}

  explicit Reader(const T& str) :
    CharPointer::Reader(str.c_str(), str.length())
  {}
};

}

#if ARDUINOJSON_ENABLE_ARDUINO_STRING
template <>
struct ReaderImplBase<String>
  : StdString::Reader<String> {

  using StdString::Reader<String>::Reader;
};

template <>
struct ReaderImplBase<StringSumHelper>
  : StdString::Reader<StringSumHelper> {

  using StdString::Reader<StringSumHelper>::Reader;
};
#endif

#if ARDUINOJSON_ENABLE_STD_STRING
template <>
struct ReaderImplBase<std::string>
  : StdString::Reader<std::string> {

  using StdString::Reader<std::string>::Reader;
};
#endif

}
}
}
