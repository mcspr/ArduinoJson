// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include <cstdint>

namespace ArduinoJson {
namespace Internals {

template <typename TInput>
bool skipBom(TInput& input) {
  static constexpr uint8_t Bom[] = {0xef, 0xbb, 0xbf};
  if (Bom[0] == static_cast<uint8_t>(input.current())) {
    input.move();

    uint8_t tmp[2];
    tmp[0] = static_cast<uint8_t>(input.current());
    tmp[1] = static_cast<uint8_t>(input.next());

    const auto out =
      Bom[1] == tmp[0] &&
      Bom[2] == tmp[1];
    if (out) {
      input.move();
      input.move();
    }

    return out;
  }

  return true;
}

template <typename TInput>
void skipSpacesAndComments(TInput& input) {
  for (;;) {
    switch (input.current()) {
      // spaces
      case ' ':
      case '\t':
      case '\r':
      case '\n':
        input.move();
        continue;

      // comments
      case '/':
        switch (input.next()) {
          // C-style block comment
          case '*':
            input.move();  // skip '/'
            // no need to skip '*'
            for (;;) {
              input.move();
              if (input.current() == '\0') return;
              if (input.current() == '*' && input.next() == '/') {
                input.move();  // skip '*'
                input.move();  // skip '/'
                break;
              }
            }
            break;

          // C++-style line comment
          case '/':
            // not need to skip "//"
            for (;;) {
              input.move();
              if (input.current() == '\0') return;
              if (input.current() == '\n') break;
            }
            break;

          // not a comment, just a '/'
          default:
            return;
        }
        break;

      default:
        return;
    }
  }
}

template <typename TInput>
bool skipUnreadable(TInput& input) {
  if (skipBom(input)) {
    skipSpacesAndComments(input);
    return true;
  }

  return false;
}

}  // namespace Internals
}  // namespace ArduinoJson
