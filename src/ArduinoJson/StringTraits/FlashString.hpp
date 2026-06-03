// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#if ARDUINOJSON_ENABLE_PROGMEM

#include "StringTraitsBase.hpp"
#include "CharPointer.hpp"

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

  static bool equals(const __FlashStringHelper* str, const char* expected) {
    const char* actual = reinterpret_cast<const char*>(str);
    if (!actual || !expected) return actual == expected;
    return strcmp_P(expected, actual) == 0;
  }

  static bool is_null(const __FlashStringHelper* str) {
    return !str;
  }

  typedef Traits::duplicate_t duplicate_t;

  template <typename Buffer>
  static duplicate_t duplicate(const __FlashStringHelper* str, Buffer* buffer) {
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

  static const bool has_append = false;
  static const bool has_equals = true;
  static const bool should_duplicate = true;
};
}  // namespace Internals
}  // namespace ArduinoJson

#endif
