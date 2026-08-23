// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

// A Print implementation that allows to write in a char[]
class StaticStringBuilder {
 public:
  StaticStringBuilder(char *ptr, size_t size) :
    _ptr(ptr),
    _end(ptr + size)
  {}

  size_t print(char c) {
    if ((_ptr + 1) < _end) {
      _ptr[0] = c;
      _ptr[1] = '\0';
      _ptr += 1;
      return 1;
    }

    return 0;
  }

  size_t print(const char *s) {
    char *begin = _ptr;
    if (_ptr < _end) {
      char *end = _end - 1;
      while ((_ptr < end) && *s) {
        *_ptr++ = *s++;
      }

      _ptr[0] = '\0';
    }

    return static_cast<size_t>(_ptr - begin);
  }

 private:
  char *_ptr;
  char *_end;
};
}  // namespace Internals
}  // namespace ArduinoJson
