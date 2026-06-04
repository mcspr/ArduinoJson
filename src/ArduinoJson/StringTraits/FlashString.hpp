// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#if ARDUINOJSON_ENABLE_PROGMEM

#include "../TypeTraits/Constant.hpp"

#include "StringTraitsBase.hpp"
#include "CharPointer.hpp"

class __FlashStringHelper;

namespace ArduinoJson {
namespace Internals {

template <>
struct StringTraitsImpl<const __FlashStringHelper*, void> {
 private:
  struct ReaderImpl {
    static char read(const void* ptr) {
      return pgm_read_byte_near(ptr);
    }
  };

  typedef CharPointerTraits<char> Traits;

 public:
  typedef Traits::ReaderBase<ReaderImpl> Reader;

  static bool equals(const void* str, const char* expected) {
    const char* actual = reinterpret_cast<const char*>(str);
    if (!actual || !expected) return actual == expected;
    return strcmp_P(expected, actual) == 0;
  }

  static bool is_null(const void*) {
    return false;
  }

  typedef Traits::duplicate_t duplicate_t;

  template <typename Buffer>
  static duplicate_t duplicate(const void* str, Buffer* buffer) {
    void* dup = nullptr;
    if (!is_null(str)) {
      const char* ptr = reinterpret_cast<duplicate_t>(str);
      size_t size = strlen_P(ptr);
      dup = buffer->alloc(size + 1);
      if (dup != nullptr) {
        memcpy_P(dup, ptr, size);
        reinterpret_cast<char *>(dup)[size] = '\0';
      }
    }

    return static_cast<duplicate_t>(dup);
  }

  typedef FalseType has_append;
  typedef TrueType has_equals;
  typedef TrueType should_duplicate;
};
}  // namespace Internals
}  // namespace ArduinoJson

#endif
