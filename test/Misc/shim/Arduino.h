#pragma once

#include <cstring>

#define memcmp_P std::memcmp
#define memcpy_P std::memcpy
#define strcmp_P std::strcmp
#define strlen_P std::strlen

inline char pgm_read_byte(const void* ptr) {
  return *reinterpret_cast<const char *>(ptr);
}

inline char pgm_read_byte_near(const void* ptr) {
  return pgm_read_byte(ptr);
}

class __FlashStringHelper;
class String;

namespace ArduinoJson {
namespace Internals {
namespace Strings {
namespace FlashString {

static constexpr inline bool Probe(const void*) {
  return true;
}

}
}
}
}
