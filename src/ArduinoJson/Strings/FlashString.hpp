// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#if ARDUINOJSON_ENABLE_PROGMEM

#include "../JsonBuffer.hpp"
#include "CharPointer.hpp"

#include <Arduino.h>

#include <cstddef>
#include <cstring>

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
  static inline char Operator(const void* str) {
    return pgm_read_byte(str);
  }

  static void Operator(char* out, const void* str, size_t len) {
    memcpy_P(out, str, len);
  }

  static void Operator(char* out, const void* str) {
    Operator(out, str, Length::Operator(str));
  }
};

namespace Impl {

struct ProbeMemcmp {
  static int Operator(const void* lhs, const void* rhs, size_t len) {
    if (Probe(lhs)) {
      return memcmp_P(rhs, lhs, len);
    } else if (Probe(rhs)) {
      return memcmp_P(lhs, rhs, len);
    }

    return Strings::CharPointer::Compare::Operator(lhs, rhs, len);
  }
};

// TODO alignment check for faster reads when possible
struct NaiveMemcmp {
  static int Operator(const void* lhs, const void* rhs, size_t len) {
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
};

struct NaiveStrcmp {
  static int Operator(const char* lhs, const char* rhs) {
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
};

struct ProbeStrcmp {
  static int Operator(const char* lhs, const char* rhs) {
    if (Probe(lhs)) {
      return strcmp_P(rhs, lhs);
    } else if (Probe(rhs)) {
      return strcmp_P(lhs, rhs);
    }

    return std::strcmp(lhs, rhs);
  }
};

}

struct Compare {
  static int Operator(const void* str, const void* other, size_t str_len) {
    if (Probe(str) && Probe(other)) {
      return Impl::NaiveMemcmp::Operator(str, other, str_len);
    }

    return Impl::ProbeMemcmp::Operator(str, other, str_len);
  }
};

struct StringCompare {
  static int Operator(const void* str, const void* other) {
    if (str == other)
      return true;

    if (Probe(str) && Probe(other))
      return Impl::NaiveStrcmp::Operator(
        reinterpret_cast<const char *>(str),
        reinterpret_cast<const char *>(other));

    return Impl::ProbeStrcmp::Operator(
      reinterpret_cast<const char *>(str),
      reinterpret_cast<const char *>(other));
  }
};

struct Equals : Strings::CharPointer::Impl::Equals<Equals> {
  using Length = FlashString::Length;
  using Compare = FlashString::Compare;
  using StringCompare = FlashString::StringCompare;
};

struct Reference {
  static const char* Operator(const void* str) {
    return reinterpret_cast<const char *>(str);
  }
};

struct Duplicate {
  static void Operator(void* dup, const void* str, size_t len) {
    memcpy_P(dup, str, len);
  }

  static const char* Operator(JsonBuffer* buffer, const void* str, size_t len) {
    void* dup = nullptr;
    if (str != nullptr) {
      auto* ptr = reinterpret_cast<const char *>(str);
      dup = buffer->alloc(len + 1);
      if (dup != nullptr) {
        Operator(dup, ptr, len);
        reinterpret_cast<char *>(dup)[len] = '\0';
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
