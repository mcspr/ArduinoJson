// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#if ARDUINOJSON_ENABLE_PROGMEM

#include <Arduino.h>

#include "../JsonBuffer.hpp"

namespace ArduinoJson {
namespace Internals {
namespace Strings {
namespace FlashString {

struct Equals {
  static bool Operator(const void* str, const char* expected) {
    const char* actual = reinterpret_cast<const char*>(str);
    if (!actual || !expected) return actual == expected;
    return strcmp_P(expected, actual) == 0;
  }
};

struct Duplicate {
  static const char* Operator(const void* str, JsonBuffer* buffer) {
    void* dup = nullptr;
    if (str != nullptr) {
      auto* ptr = reinterpret_cast<const char *>(str);
      size_t size = strlen_P(ptr);
      dup = buffer->alloc(size + 1);
      if (dup != nullptr) {
        memcpy_P(dup, ptr, size);
        reinterpret_cast<char *>(dup)[size] = '\0';
      }
    }

    return static_cast<const char*>(dup);
  }
};

}  // namespace FlashString
}  // namespace Strings
}  // namespace Internals
}  // namespace ArduinoJson

#endif
