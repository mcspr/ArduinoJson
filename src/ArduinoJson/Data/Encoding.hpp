// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include <cstdint>
#include <cstddef>

namespace ArduinoJson {
namespace Internals {

class Encoding {
 public:
  // Optimized for code size on a 8-bit AVR
  static char escapeChar(char c) {
    const char* p = escapeTable();
    while (p[1] && static_cast<uint8_t>(c) < static_cast<uint8_t>(p[1]))
      p += 2;

    if (p[1] && p[1] == c)
      return p[0];

    return '\0';
  }

  // Optimized for code size on a 8-bit AVR
  static char unescapeChar(char c) {
    const char* p = unescapeTable();
    for (;;) {
      if (p[0] == '\0')
        return '\0';
      if (p[0] == c)
        return p[1];
      p += 2;
    }
  }

 private:
  static constexpr size_t EscapeOffset = 4;
  static constexpr size_t UnescapeOffset = 0;

  static const char* characterTable(size_t offset) {
    alignas(2) static constexpr char table[] = {
      // Unescape table offset
      '/', '/',
      '\'', '\'',
      // Escape table offset
      // Sorted in descending order to allow early exit in escapeChar()
      '\\', '\\',
      '"', '"',
      'r', '\r',
      'f', '\f',
      'n', '\n',
      't', '\t',
      'b', '\b',
      '\0', '\0',
    };

    return &table[offset];
  }

  static const char* escapeTable() {
    return characterTable(EscapeOffset);
  }

  static const char* unescapeTable() {
    return characterTable(UnescapeOffset);
  }
};

}  // namespace Internals
}  // namespace ArduinoJson
