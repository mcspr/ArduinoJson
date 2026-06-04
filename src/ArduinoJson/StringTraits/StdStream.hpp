// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#if ARDUINOJSON_ENABLE_STD_STREAM

#include "StringTraitsBase.hpp"

#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsBaseOf.hpp"
#include "../TypeTraits/RemoveReference.hpp"

#include <istream>

namespace ArduinoJson {
namespace Internals {

struct StdStreamTraits {
  class Reader {
    std::istream& _stream;
    char _current, _next;

   public:
    Reader(std::istream& stream) : _stream(stream), _current(0), _next(0) {}

    void move() {
      _current = _next;
      _next = 0;
    }

    char current() {
      if (!_current) _current = read();
      return _current;
    }

    char next() {
      // assumes that current() has been called
      if (!_next) _next = read();
      return _next;
    }

   private:
    char read() {
      return _stream.eof() ? '\0' : static_cast<char>(_stream.get());
    }
  };

  typedef FalseType has_append;
  typedef FalseType has_equals;
};

template <typename TStream>
struct StringTraitsImpl<
    TStream,
    // match any type that is derived from std::istream:
    typename EnableIf<IsBaseOf<
        std::istream, typename RemoveReference<TStream>::type>::value>::type>
    : StdStreamTraits {};
}  // namespace Internals
}  // namespace ArduinoJson

#endif
