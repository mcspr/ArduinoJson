// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#if ARDUINOJSON_ENABLE_STD_STREAM

#include "Base.hpp"

#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsBaseOf.hpp"

#include <istream>

namespace ArduinoJson {
namespace Internals {
namespace Readers {
namespace Istream {

class Reader {
  std::istream& _stream;
  char _current{};
  char _next{};

 public:
  explicit Reader(std::istream& stream) :
    _stream(stream)
  {}

  void move() {
    _current = _next;
    _next = 0;
  }

  char current() {
    if (!_current)
      _current = read();
    return _current;
  }

  char next() {
    // assumes that current() has been called
    if (!_next)
      _next = read();
    return _next;
  }

 private:
  char read() {
    return _stream.eof() ? '\0' : static_cast<char>(_stream.get());
  }
};

}

template <typename T>
struct ReaderImplBase<T,
  typename EnableIf<IsBaseOf<std::istream, T>::value>::type> : Istream::Reader {

  using Istream::Reader::Reader;
};

}  // namespace Readers
}  // namespace Internals
}  // namespace ArduinoJson

#endif
