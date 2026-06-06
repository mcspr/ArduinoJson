// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

namespace ArduinoJson {
namespace Internals {

class Encoding {
 public:
  // Optimized for code size on a 8-bit AVR
  static char escapeChar(char c) {
    const char *p = escapeTable(EscapeTableSerializing);
    while (p[0] && p[1] != c) {
      p += 2;
    }
    return p[0];
  }

  // Optimized for code size on a 8-bit AVR
  static char unescapeChar(char c) {
    const char *p = escapeTable(EscapeTableDeserializing);
    for (;;) {
      if (p[0] == '\0')
        return '\0';
      if (p[0] == c)
        return p[1];
      p += 2;
    }
  }

 private:
  static constexpr int EscapeTableSerializing = 4;
  static constexpr int EscapeTableDeserializing = 0;

  static const char *escapeTable(int offset) {
    return &"//''\"\"\\\\b\bf\fn\nr\rt\t"[offset];
  }
};

}  // namespace Internals
}  // namespace ArduinoJson
