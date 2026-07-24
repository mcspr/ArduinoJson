// MIT License

#pragma once

#include "../JsonBuffer.hpp"

#include <cstddef>
#include <cstring>

namespace ArduinoJson {
namespace Internals {
namespace Strings {
namespace CharPointer {

struct Length {
  static size_t Operator(const void* str) {
    return std::strlen(reinterpret_cast<const char *>(str));
  }

  template <typename TChar, size_t Size>
  static size_t Operator(TChar (&)[Size]) {
    return Size - 1;
  }
};

struct Copy {
  static char Operator(const void* str) {
    return *reinterpret_cast<const char *>(str);
  }

  static void Operator(char* out, const void* str, size_t len) {
    std::memcpy(out, str, len);
  }

  static void Operator(char* out, const void* str) {
    Operator(out, str, Length::Operator(str));
  }
};

struct IsNull {
  static bool Operator(const void* str) {
    return !str;
  }
};

struct Reference {
  static const char* Operator(const void* str) {
    return reinterpret_cast<const char *>(str);
  }

  template <typename TChar, size_t Size>
  static const char* Operator(TChar (&str)[Size]) {
    return Operator(&str[0]);
  }
};

struct Equals {
  static bool Operator(const void* str, size_t str_len, const char* expected, size_t expected_len) {
    const char* actual = reinterpret_cast<const char *>(str);
    if (!actual || !expected)
      return actual == expected;
    if (str_len == expected_len)
      return std::memcmp(actual, expected, str_len) == 0;
  
    return false;
  }

  template <typename TActual, size_t ActualSize, typename TExpected, size_t ExpectedSize>
  static bool Operator(TActual (&actual)[ActualSize], TExpected (&expected)[ExpectedSize]) {
    return Operator(&actual[0], ActualSize - 1, &expected[0], ExpectedSize - 1);
  }

  template <typename TChar, size_t Size>
  static bool Operator(TChar (&actual)[Size], const char* expected, size_t expected_len) {
    return Operator(&actual[0], Size - 1, expected, expected_len);
  }

  static bool Operator(const void* str, const char* expected, size_t expected_len) {
    const char* actual = reinterpret_cast<const char *>(str);
    if (!actual || !expected)
      return actual == expected;
    return Operator(actual, Length::Operator(str), expected, expected_len);
  }
  
  static bool Operator(const void* str, size_t str_len, const char* expected) {
    if (!expected)
      return str_len == 0 || str == expected;
  
    return Operator(str, str_len, expected, Length::Operator(expected));
  }
  
  static bool Operator(const void* str, const char* expected) {
    const char* actual = reinterpret_cast<const char *>(str);
    if (!actual || !expected)
      return actual == expected;
    return std::strcmp(actual, expected) == 0;
  }
};

struct Duplicate {
  static void Operator(void* dup, const void* str, size_t len) {
    std::memcpy(dup, str, len);
    reinterpret_cast<char *>(dup)[len] = '\0';
  }

  static const char* Operator(JsonBuffer* buffer, const void* str, size_t len) {
    void* dup = nullptr;
    if (str) {
      dup = buffer->alloc(len + 1);
      if (dup != nullptr) {
        Operator(dup, str, len);
      }
    }
  
    return static_cast<const char *>(dup);
  }

  template <typename TChar, size_t Size>
  static const char* Operator(JsonBuffer* buffer, TChar (&str)[Size]) {
    return Operator(buffer, &str[0], Size - 1);
  }

  static const char* Operator(JsonBuffer* buffer, const void* str) {
    if (str)
      return Operator(buffer, str, Length::Operator(str));

    return nullptr;
  }
};

}
}
}
}
