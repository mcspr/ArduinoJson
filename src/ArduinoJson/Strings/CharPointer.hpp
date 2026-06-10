// MIT License

#pragma once

#include "../JsonBuffer.hpp"

#include <cstddef>
#include <cstring>

namespace ArduinoJson {
namespace Internals {
namespace Strings {
namespace CharPointer {

struct Equals {
  static bool Operator(const void* str, size_t str_len, const char* expected, size_t expected_len) {
    const char* actual = reinterpret_cast<const char*>(str);
    if (!actual || !expected)
      return actual == expected;
    if (str_len == expected_len)
      return memcmp(actual, expected, str_len) == 0;
  
    return false;
  }
  
  static bool Operator(const void* str, size_t str_len, const char* expected) {
    if (!expected)
      return str_len == 0 || str == expected;
  
    return Operator(str, str_len, expected, strlen(expected));
  }
  
  static bool Operator(const void* str, const char* expected) {
    const char* actual = reinterpret_cast<const char*>(str);
    if (!actual || !expected) return actual == expected;
    return strcmp(actual, expected) == 0;
  }
};

struct IsNull {
  static bool Operator(const void* str) {
    return !str;
  }
};

struct Duplicate {
  static const char* Operator(const void* str, JsonBuffer* buffer, size_t size) {
    void* dup = nullptr;
    if (str) {
      dup = buffer->alloc(size + 1);
      if (dup != nullptr) {
        memcpy(dup, str, size);
        reinterpret_cast<char*>(dup)[size] = '\0';
      }
    }
  
    return static_cast<const char*>(dup);
  }
  
  static const char* Operator(const void* str, JsonBuffer* buffer) {
    if (str)
      return Operator(str, buffer, strlen(reinterpret_cast<const char*>(str)));

    return nullptr;
  }
  
  
  template <typename TChar, size_t Size>
  static const char* Operator(TChar (&str)[Size], JsonBuffer* buffer) {
    return Operator(&str[0], buffer, Size - 1);
  }
};

}
}
}
}
