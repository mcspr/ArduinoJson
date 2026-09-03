// MIT License

#pragma once

#include "../Configuration.hpp"

#include "Base.hpp"
#include "CharPointer.hpp"

#if ARDUINOJSON_ENABLE_STD_STRING
#include <string>
#endif

namespace ArduinoJson {
namespace Internals {
namespace Readers {
namespace StdString {

template <typename T>
struct Reader : Readers::CharPointer::Reader {
  explicit Reader(T& str) noexcept :
    CharPointer::Reader(str.c_str(), str.length()),
    _str(str)
  {}

  explicit Reader(const T& str) noexcept :
    CharPointer::Reader(str.c_str(), str.length()),
    _str(str)
  {}

  Reader(const Reader&) = delete;
  Reader(Reader&& other) noexcept :
    CharPointer::Reader(other._str.c_str(), other._str.length()),
    _str(other._str)
  {}

  Reader& operator=(const Reader&) = delete;
  Reader& operator=(Reader&&) = delete;

 private:
  const T& _str;
};

}

#if ARDUINOJSON_ENABLE_STD_STRING
template <>
struct ReaderImplBase<std::string> {
  using type = StdString::Reader<std::string>;
};
#endif

}
}
}
