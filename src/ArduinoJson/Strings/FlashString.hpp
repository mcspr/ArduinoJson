// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#if ARDUINOJSON_ENABLE_PROGMEM

#include <Arduino.h>

#include <cstddef>
#include <cstring>

#include "../JsonBuffer.hpp"

namespace ArduinoJson {
namespace Internals {
namespace Strings {
namespace FlashString {

#if defined(ESP8266)
// anything above 0x4xxxxxx should use pgmspace.h helpers
static constexpr uintptr_t FlashStringAddressMask { 1 << 30 };

static inline bool __attribute__((const)) Probe(const void*);
static inline bool Probe(const void* ptr) {
  return (reinterpret_cast<uintptr_t>(ptr) & FlashStringAddressMask) > 0;
}
#elif defined(ESP32)
// sdk already puts strings in flash (...w/ an exception of some other explicit things like psram)
static constexpr inline bool Probe(const void*) {
  return false;
}
#elif !ARDUINOJSON_PROGMEM_PROBE_IMPLEMENTED
static constexpr inline bool Probe(const void*) {
  return false;
}
#endif

struct Length {
  static size_t Operator(const void* str) {
    return strlen_P(reinterpret_cast<const char *>(str));
  }
};

struct Copy {
  static inline char ARDUINOJSON_FORCE_INLINE Operator(const void* str) {
    return pgm_read_byte(str);
  }

  static void Operator(char* out, const void* str, size_t len) {
    memcpy_P(out, str, len);
  }

  static void Operator(char* out, const void* str) {
    Operator(out, str, Length::Operator(str));
  }
};

struct Reference {
  static const char* Operator(const void* str) {
    return reinterpret_cast<const char *>(str);
  }
};

struct Equals {
  static bool Operator(const void* str, size_t str_len, const char* expected, size_t expected_len) {
    if (str_len == expected_len) {
      if (str == expected)
        return true;

      if (Probe(str) && Probe(expected)) {
        return NaiveMemcmp(str, expected, str_len) == 0;
      }

      return ProbeMemcmpEqual(str, expected, str_len);
    }

    return false;
  }

  static bool Operator(const void* str, size_t str_len, const char* expected) {
    if (!expected)
      return str_len == 0 || str == expected;

    return Operator(str, str_len, expected, Length::Operator(expected));
  }

  static bool Operator(const void* str, const char* expected, size_t expected_len) {
    const char* actual = reinterpret_cast<const char *>(str);
    if (!actual || !expected)
      return actual == expected;
    return Operator(actual, Length::Operator(str), expected, expected_len);
  }

  static bool Operator(const void* str, const char* expected) {
    const char* actual = reinterpret_cast<const char*>(str);
    if (!actual || !expected)
      return actual == expected;

    if (actual == expected)
      return true;

    if (Probe(actual) && Probe(expected))
      return NaiveStrcmp(actual, expected);

    return ProbeStrcmpEqual(actual, expected);
  }

 private:
  // TODO alignment check for faster reads when possible
  static int NaiveMemcmp(const void* lhs, const void* rhs, size_t len) {
    size_t offset = 0;

    auto* lhs_ptr = reinterpret_cast<const char *>(lhs);
    auto* rhs_ptr = reinterpret_cast<const char *>(rhs);

    while (offset != len) {
      const auto lhs_char =
        static_cast<unsigned char>(Copy::Operator(lhs_ptr + offset));
      const auto rhs_char =
        static_cast<unsigned char>(Copy::Operator(rhs_ptr + offset));

      if (lhs_char != rhs_char)
        return lhs_char - rhs_char;

      ++offset;
    }

    return 0;
  }

  static bool ProbeMemcmpEqual(const void* lhs, const void* rhs, size_t len) {
    if (Probe(lhs)) {
      return memcmp_P(rhs, lhs, len) == 0;
    } else if (Probe(rhs)) {
      return memcmp_P(lhs, rhs, len) == 0;
    }

    return std::memcmp(lhs, rhs, len) == 0;
  }

  static int NaiveStrcmp(const char* lhs, const char* rhs) {
    size_t offset = 0;

    for (;;) {
      const auto lhs_char =
        static_cast<unsigned char>(Copy::Operator(lhs + offset));
      const auto rhs_char =
        static_cast<unsigned char>(Copy::Operator(rhs + offset));

      if ((!lhs_char || !rhs_char) ||
          (lhs_char != rhs_char))
        return lhs_char - rhs_char;

      ++offset;
    }

    return 0;
  }

  static bool ProbeStrcmpEqual(const char* lhs, const char* rhs) {
    if (Probe(lhs)) {
      return strcmp_P(rhs, lhs) == 0;
    } else if (Probe(rhs)) {
      return strcmp_P(lhs, rhs) == 0;
    }

    return std::strcmp(lhs, rhs) == 0;
  }
};

struct Duplicate {
  static void Operator(void* dup, const void* str, size_t len) {
    memcpy_P(dup, str, len);
    reinterpret_cast<char *>(dup)[len] = '\0';
  }

  static const char* Operator(JsonBuffer* buffer, const void* str, size_t len) {
    void* dup = nullptr;
    if (str != nullptr) {
      auto* ptr = reinterpret_cast<const char *>(str);
      dup = buffer->alloc(len + 1);
      if (dup != nullptr) {
        Operator(dup, ptr, len);
      }
    }

    return static_cast<const char*>(dup);
  }

  static const char* Operator(JsonBuffer* buffer, const void* str) {
    return Operator(buffer, str, Length::Operator(str));
  }
};

}  // namespace FlashString
}  // namespace Strings
}  // namespace Internals
}  // namespace ArduinoJson

#endif
