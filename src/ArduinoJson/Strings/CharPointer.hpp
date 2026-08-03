// MIT License

#pragma once

#include "../JsonBuffer.hpp"

#include <cstddef>
#include <cstring>

namespace ArduinoJson {
namespace Internals {
namespace Strings {
namespace CharPointer {
namespace Impl {

template <typename TImpl>
struct Equals {
  static bool Operator(const void* str, size_t str_len, const char* expected, size_t expected_len) {
    if (str_len == expected_len) {
      if ((str_len == 0 ) || (str == expected))
        return true;

      return TImpl::Compare::Operator(str, expected, str_len) == 0;
    }

    return false;
  }

  template <typename TChar, size_t Size, typename TExpected, size_t ExpectedSize>
  static bool Operator(TChar (&str)[Size], TExpected (&expected)[ExpectedSize]) {
    return Operator(&str[0], Size - 1, &expected[0], ExpectedSize - 1);
  }

  template <typename TChar, size_t Size>
  static bool Operator(TChar (&str)[Size], const char* expected, size_t expected_len) {
    return Operator(&str[0], Size - 1, expected, expected_len);
  }

  static bool Operator(const void* str, const char* expected, size_t expected_len) {
    if (!str)
      return str == expected;

    const auto str_len = TImpl::Length::Operator(str);
    if ((str_len == 0) || (str == expected))
      return true;

    return Operator(str, str_len, expected, expected_len);
  }

  static bool Operator(const void* str, size_t str_len, const char* expected) {
    if (!expected)
      return (str_len == 0) || (str == expected);

    const auto expected_len = TImpl::Length::Operator(expected);
    if (str_len == expected_len)
      return Operator(str, str_len, expected, expected_len);

    return false;
  }

  static bool Operator(const void* str, const char* expected) {
    if (!str || !expected)
      return str == expected;

    return TImpl::StringCompare::Operator(str, expected) == 0;
  }
};

}

struct Length {
  static size_t Operator(const void* str) {
    return std::strlen(reinterpret_cast<const char *>(str));
  }

  template <typename TChar, size_t Size>
  static size_t Operator(TChar (&)[Size]) {
    return Size - 1;
  }
};

struct Compare {
  static int Operator(const void* str, const void* other, size_t len) {
    return std::memcmp(str, other, len);
  }
};

struct StringCompare {
  static int Operator(const void* str, const void* other) {
    return std::strcmp(
      reinterpret_cast<const char *>(str),
      reinterpret_cast<const char *>(other));
  }
};

struct Equals : Impl::Equals<Equals> {
  using Length = CharPointer::Length;
  using Compare = CharPointer::Compare;
  using StringCompare = CharPointer::StringCompare;
};

struct Copy {
  static char Operator(const void* str) {
    return *reinterpret_cast<const char *>(str);
  }

  static void Operator(void* out, const void* str, size_t len) {
    std::memcpy(out, str, len);
  }

  static void Operator(void* out, const void* str) {
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

struct Duplicate {
  static void Operator(void* dup, const void* str, size_t len) {
    std::memcpy(dup, str, len);
  }

  static const char* Operator(JsonBuffer* buffer, const void* str, size_t len) {
    void* dup = nullptr;
    if (str) {
      dup = buffer->alloc(len + 1);
      if (dup != nullptr) {
        Operator(dup, str, len);
        reinterpret_cast<char *>(dup)[len] = '\0';
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
