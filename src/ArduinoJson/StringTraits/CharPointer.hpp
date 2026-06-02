// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../TypeTraits/IsConst.hpp"

#include <cstddef>
#include <cstdio>

namespace ArduinoJson {
namespace Internals {

template <typename TChar>
struct CharPointerTraits {
  class Reader {
    const TChar* _ptr;
    const TChar* _end;

   public:
    Reader(const TChar* ptr, size_t size) :
      _ptr(ptr ? ptr : reinterpret_cast<const TChar*>("")),
      _end(ptr ? (ptr + size) : 0)
    {}

    void move() {
      if (_ptr < _end)
        ++_ptr;
    }

    char current() const {
      if (_ptr < _end)
        return char(_ptr[0]);

      return '\0';
    }

    char next() const {
      if ((_ptr + 1) < _end)
        return char(_ptr[1]);

      return '\0';
    }
  };

  static bool equals(const TChar* str, const char* expected) {
    const char* actual = reinterpret_cast<const char*>(str);
    if (!actual || !expected) return actual == expected;
    return strcmp(actual, expected) == 0;
  }

  static bool is_null(const TChar* str) {
    return !str;
  }

  typedef const char* duplicate_t;

  template <typename Buffer>
  static duplicate_t duplicate(const TChar* str, Buffer* buffer, size_t size) {
    void* dup = nullptr;
    if (!is_null(str)) {
      dup = buffer->alloc(size + 1);
      if (dup != nullptr) {
        memcpy(dup, str, size);
        reinterpret_cast<TChar *>(dup)[size] = '\0';
      }
    }

    return static_cast<duplicate_t>(dup);
  }

  template <typename Buffer>
  static duplicate_t duplicate(const TChar* str, Buffer* buffer) {
    return duplicate(str, buffer, strlen(reinterpret_cast<const char *>(str)));
  }

  static const bool has_append = false;
  static const bool has_equals = true;
  static const bool should_duplicate = !IsConst<TChar>::value;
};

// char*, unsigned char*, signed char*
// const char*, const unsigned char*, const signed char*
template <typename TChar>
struct StringTraitsImpl<TChar*, typename EnableIf<IsChar<TChar>::value>::type>
    : CharPointerTraits<TChar> {};

// char[], unsigned char[], signed char[]
// const char[], const unsigned char[], const signed char[]
template <typename TChar, size_t Size>
struct StringTraitsImpl<TChar[Size], typename EnableIf<IsChar<TChar>::value>::type>
    : CharPointerTraits<TChar> {

  template <typename Buffer>
  static typename CharPointerTraits<TChar>::duplicate_t
  duplicate(const TChar* str, Buffer* buffer) {
    return CharPointerTraits<TChar>::duplicate(str, buffer, Size - 1);
  }
};

}  // namespace Internals
}  // namespace ArduinoJson
