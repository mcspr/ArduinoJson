// MIT License

#pragma once

#include "../Configuration.hpp"
#include "../Polyfills/expect.hpp"

#include <cstdint>

namespace ArduinoJson {
namespace Internals {
namespace Unicode {

// ref. https://github.com/protocolbuffers/protobuf/blob/main/third_party/utf8_range/utf8_range.c (MIT License)
// EVERY character input is validated, not just the \\uxxxx escaped ones
// NOT particularly optimimized, table state machine or simd would obviously be faster (when supported by the machine)
namespace Utf8 {

// maximum number of code units, since the character length is not constant
struct Codepoint {
  uint8_t value[4]{};
};

struct NoopState {
  static constexpr bool append(char) {
    return true;
  }

  constexpr explicit operator bool() const {
    return true;
  }
};

struct BranchingState {
  bool append(char value) {
    uint8_t c = static_cast<uint8_t>(value);

    // [00..7F] ASCII -> just one byte
    if (ARDUINOJSON_LIKELY(lead == '\0' && c < 0x80))
      return true;

    // Check whether the lead and trailing bytes (1..3) produce "Well-Formed UTF-8 Byte Sequences"
    if (seq) {
      bool out = false;

      if (next == '\0') {
        next = c;

        // Note that none of the trailing bytes could be in ASCII range or be too large
        // But, currently there's no specific error code / state to return
        switch (seq) {
        case 3:
          out = ARDUINOJSON_LIKELY(
            ((lead == 0xF0 && next >= 0x90) ||  //       F0 [90..BF] [80..BF] [80..BF]
             (lead >= 0xF1 && lead <= 0xF3) ||  // [F1..F3] [80..BF] [80..BF] [80..BF]
             (lead == 0xF4 && next <= 0x8F)));  //       F4 [80..BF] [80..BF] [80..BF]
          break;

        case 2:
          out = ARDUINOJSON_LIKELY(
            ((lead == 0xE0 && next >= 0xA0) ||  //       E0 [A0..BF] [80..BF]
             (lead >= 0xE1 && lead <= 0xEC) ||  // [E1..EC] [80..BF] [80..BF]
             (lead == 0xED && next <= 0x9F) ||  //       ED [80..9F] [80..BF]
             (lead >= 0xEE && lead <= 0xEF)));  // [EE..EF] [80..BF] [80..BF]
          break;

        case 1:
          goto VALIDATE_TRAILING_BYTE;
        
        default:
          __builtin_unreachable();

        }

      } else {
VALIDATE_TRAILING_BYTE:
        out = ARDUINOJSON_LIKELY((c & 0xC0) == 0x80);  // [80..BF]

      }

      if (!--seq) {
        lead = '\0';
        next = '\0';
      }

      return out;
    }

    // Specific lead byte range results in a specific number of (possible) trailing bytes
    // The block above would validate lead range again after encountering the next byte
    if (ARDUINOJSON_LIKELY(c >= 0xC2 && c <= 0xDF)) {
      lead = c;
      if (lead >= 0xF0)
        seq = 3;
      else if (lead >= 0xE0)
        seq = 2;
      else
        seq = 1;

      return true;
    }

    return false;
  }

  explicit operator bool() const {
    return seq == 0;  // incomplete sequence of trailing bytes
  }

  uint8_t lead{};  // aka first byte of the sequence
  uint8_t next{};  // aka second byte of the sequence
  uint8_t seq{};   // expected number of bytes *after* lead
};

// nb. 7.x immediately writes to the ouput instead of converting value into a byte range first
// converts aggregated bytes from one or two Utf16::Codepoint(s) into an UTF-8 byte sequence
inline Codepoint encode(uint32_t value) {
  Codepoint out;

  if (value < 0x80) {
    out.value[3] = static_cast<uint8_t>(value);

  } else {
    auto* p = &out.value[3];
    *(p--) = static_cast<uint8_t>((value | 0x80) & 0xBF);

    uint16_t remaining = uint16_t(value >> 6);
    if (remaining < 0x20) {  // 0x800
      *(p--) = static_cast<uint8_t>(value | 0xC0);

    } else {
      *(p--) = static_cast<uint8_t>((remaining | 0x80) & 0xBF);
      remaining = static_cast<uint16_t>(remaining >> 6);
      if (remaining < 0x10) {  // 0x10000
        *(p--) = static_cast<uint8_t>(remaining | 0xE0);

      } else {
        *(p--) = static_cast<uint8_t>((remaining | 0x80) & 0xBF);
        remaining = static_cast<uint16_t>(remaining >> 6);
        *(p--) = static_cast<uint8_t>(remaining | 0xF0);

      }
    }
  }

  return out;
}

using State =
#if ARDUINOJSON_VALIDATE_UTF8
 BranchingState;
#else
 NoopState;
#endif

}

// ref. https://github.com/bblanchon/ArduinoJson/blob/7.x/src/ArduinoJson/Json/Utf16.hpp
// ref. https://github.com/bblanchon/ArduinoJson/blob/7.x/src/ArduinoJson/Json/Utf8.hpp
// parser *should* expect data that is marshalled using ascii-only json serializers
// (e.g. Python's json module that likes to do this by default and encode strings w/ utf16)
namespace Utf16 {

static constexpr bool isHighSurrogate(uint16_t codeunit) {
  return codeunit >= 0xD800 && codeunit < 0xDC00;
}

static constexpr bool isLowSurrogate(uint16_t codeunit) {
  return codeunit >= 0xDC00 && codeunit < 0xE000;
}

// filters through the codeunit inputs and prepares 32bit-wide codepoint for utf8 encoder
// value is aggregated by continuously calling append(), since escape sequence only captures a single 16bit chuck
class AggregatingCodepoint {
 public:
  bool append(uint16_t codeunit) {
    if (ARDUINOJSON_UNLIKELY(isHighSurrogate(codeunit))) {
      _highSurrogate = codeunit & 0x3FF;
      return false;

    } else if (ARDUINOJSON_UNLIKELY(isLowSurrogate(codeunit))) {
      _codepoint = static_cast<uint32_t>(
        0x10000 +
        ((static_cast<uint32_t>(_highSurrogate) << 10) |
         (static_cast<uint32_t>(codeunit) & 0x3FF)));

    } else {
      _codepoint = codeunit;

    }

    return true;
  }

  uint32_t value() const {
    return _codepoint;
  }

 private:
  uint16_t _highSurrogate{};
  uint32_t _codepoint{};
};

class NoopCodepoint {
 public:
  bool append(uint16_t codeunit) {
    _codepoint = codeunit;
    return true;
  }

  uint32_t value() const {
    return _codepoint;
  }

 private:
  uint32_t _codepoint;
};

using Codepoint =
#if ARDUINOJSON_DECODE_UNICODE
 AggregatingCodepoint;
#else
 NoopCodepoint;
#endif

}

}  // namespace Unicode
}  // namespace Internals
}  // namespace ArduinoJson
