// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../TypeTraits/Constant.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsChar.hpp"
#include "../TypeTraits/IsConst.hpp"
#include "../TypeTraits/Not.hpp"

#include "../Strings/Reader.hpp"

#include "StringTraitsBase.hpp"

#include <cstddef>
#include <cstring>

namespace ArduinoJson {
namespace Internals {

struct CharPointerReaderImpl {
  static char read(const void* ptr) {
    return *reinterpret_cast<const char *>(ptr);
  }
};

typedef ReaderBase<CharPointerReaderImpl> CharPointerReader;

template <typename TChar>
struct CharPointerTraits {
 public:
  typedef CharPointerReader Reader;

  static bool equals(const TChar* str, size_t str_len, const char* expected, size_t expected_len) {
    const char* actual = reinterpret_cast<const char*>(str);
    if (!actual || !expected)
      return actual == expected;
    if (str_len == expected_len)
      return memcmp(actual, expected, str_len) == 0;

    return false;
  }

  static bool equals(const TChar* str, size_t str_len, const char* expected) {
    if (!expected)
      return str_len == 0 || str == expected;

    return equals(str, str_len, expected, strlen(expected));
  }

  static bool equals(const TChar* str, const char* expected) {
    const char* actual = reinterpret_cast<const char*>(str);
    if (!actual || !expected) return actual == expected;
    return strcmp(actual, expected) == 0;
  }

  static bool is_null(const TChar* str) {
    return !str;
  }

  typedef const char* duplicate_type;

  template <typename Buffer>
  static duplicate_type duplicate(const TChar* str, Buffer* buffer, size_t size) {
    void* dup = nullptr;
    if (!is_null(str)) {
      dup = buffer->alloc(size + 1);
      if (dup != nullptr) {
        memcpy(dup, str, size);
        reinterpret_cast<TChar *>(dup)[size] = '\0';
      }
    }

    return static_cast<duplicate_type>(dup);
  }

  template <typename Buffer>
  static duplicate_type duplicate(const TChar* str, Buffer* buffer) {
    return duplicate(str, buffer, strlen(reinterpret_cast<const char *>(str)));
  }

  typedef FalseType has_append;
  typedef TrueType has_equals;
  typedef Not<IsConst<TChar>> should_duplicate;
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
  static typename CharPointerTraits<TChar>::duplicate_type
  duplicate(const TChar* str, Buffer* buffer) {
    return CharPointerTraits<TChar>::duplicate(str, buffer, Size - 1);
  }
};

}  // namespace Internals
}  // namespace ArduinoJson
