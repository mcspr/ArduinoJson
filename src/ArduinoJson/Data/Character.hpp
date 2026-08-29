// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include <cstdint>
#include <cstddef>

namespace ArduinoJson {
namespace Internals {

static constexpr bool isBetween(char c, char min, char max) {
  return min <= c && c <= max;
}

static constexpr bool canBeInNonQuotedString(char c) {
  return isBetween(c, '0', '9') || isBetween(c, '_', 'z') || isBetween(c, 'A', 'Z') ||
         c == '+' || c == '-' || c == '.';
}

static constexpr bool isQuote(char c) {
  return c == '\'' || c == '\"';
}

static constexpr bool canBeInQuotedString(char c) {
  return (static_cast<uint8_t>(c) > static_cast<uint8_t>(0x1f));  // control character should always be encoded as unicode
}

namespace Character {
namespace Deserialization {

// ref. https://github.com/stedonet/chex by Tero 'stedo' Liukko (MIT License)
// make sure to always probe for [30..39] [41..46] [61..66] before nibble -> u8 conversion
static constexpr bool isNibble(char c) {
  return (static_cast<uint8_t>(static_cast<uint8_t>(c) - '0') <= 9) ||              // [0..9]
    (static_cast<uint8_t>((static_cast<uint8_t>(c | 0x20) - 'a')) <= (0xf - 0xa));  // [a..f]
}

// assume that character is in the valid nibble range, since we only care about the lower 4 bits
// expected to be used in the parser accompanied by isNibble
static constexpr uint8_t fromNibble(char c) {
  return static_cast<uint8_t>(
    static_cast<uint8_t>(static_cast<uint8_t>(c) & 0xf) +
    static_cast<uint8_t>((static_cast<uint8_t>(c) >> 6) * 9));
}

struct Nibbles {
  uint8_t lhs;
  uint8_t rhs;

  constexpr bool ok() const {
    return ((lhs | rhs) & 0xf0) == 0;
  }

  constexpr uint8_t value() const {
    return static_cast<uint8_t>(lhs << 4) | rhs;
  }
};

static constexpr Nibbles fromNibbles(char lhs, char rhs) {
  return Nibbles{ fromNibble(lhs), fromNibble(rhs), };
}

}  // namespace Deserialization

namespace Serialization {

struct Nibbles {
  explicit constexpr Nibbles(char value) :
    _value(static_cast<uint8_t>(value))
  {}

  constexpr char left() const{
    return hexadecimal(static_cast<uint8_t>(_value >> 4) & Mask);
  }

  constexpr char right() const{
    return hexadecimal(static_cast<uint8_t>(_value & Mask));
  }

private:
  static constexpr auto Mask = uint8_t{ 0xf };
  static constexpr char hexadecimal(uint8_t nibble) {
    return "0123456789abcdef"[nibble < 16 ? nibble : 0];
  }

  uint8_t _value;
};

static constexpr Nibbles toNibbles(char c) {
  return Nibbles(c);
}

}  // namespace Serialization

class Backslash {
 public:
  struct Escaped {
    char value[8] = {{}};

    explicit operator bool() const {
      return value[0] != '\0';
    }
  };

  static Escaped escapeChar(char c) {
    Escaped out;
    const auto u8_c = static_cast<uint8_t>(c);

    auto* p = escapeTable();
    while (p[1] && u8_c < static_cast<uint8_t>(p[1]))
      p += 2;

    auto* it = &out.value[0];

    // predetermined list of displayable characters that must be escaped,
    // as well as some control characters that can avoid unicode encoding
    if (p[1] != '\0' && p[1] == c) {
      *(it++) = '\\';
      *(it++) = p[0];

    // control character should always be encoded as unicode
    } else if (u8_c < 0x20) {
      *(it++) = '\\';
      *(it++) = 'u';
      *(it++) = '0';
      *(it++) = '0';

      const auto nibbles = Serialization::toNibbles(c);
      *(it++) = nibbles.left();
      *(it++) = nibbles.right();
    }

    *(it++) = '\0';

    return out;
  }

  static char unescapeChar(char c) {
    const char* p = unescapeTable();
    while (p[0]) {
      if (p[0] == c)
        return p[1];
      p += 2;
    }

    return '\0';
  }

 private:
  static constexpr size_t EscapeOffset = 4;
  static constexpr size_t UnescapeOffset = 0;

  // Original code optimized for code size on a 8-bit AVR
  // Expecting linear search of the character counterparts
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

}  // namespace Character
}  // namespace Internals
}  // namespace ArduinoJson
