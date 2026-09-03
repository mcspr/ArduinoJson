// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include <cstdint>

namespace ArduinoJson {
namespace Internals {

// ref. RFC 8259 8.1. Character encoding
//
// > Implementations MUST NOT add a byte order mark (U+FEFF) to the
// > beginning of a networked-transmitted JSON text.  In the interests of
// > interoperability, implementations that parse JSON texts MAY ignore
// > the presence of a byte order mark rather than treating it as an
// > error.
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

// ref. RFC 8259 2. - JSON Grammar
//
// > Insignificant whitespace is allowed before or after any of the six
// > structural characters.
//
// >    ws = *(
// >            %x20 /              ; Space
// >            %x09 /              ; Horizontal tab
// >            %x0A /              ; Line feed or New line
// >            %x0D )              ; Carriage return
//
// Returns true when input contained whitespace, false otherwise
template <typename TInput>
bool skipSpaces(TInput& input) {
  bool foundSomething = false;
  for (;;) {
    switch (input.current()) {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        input.move();
        foundSomething = true;
        continue;

      default:
        break;
    }

    break;
  }

  return foundSomething;
}

enum class SkipCommentsResult {
  Nothing,
  FoundPartial,
  FoundSomething,
  Incomplete,
};

// skip everything matching single-line and multi-line comment blocks and / or lines (ref. https://jsonc.org/)
// definition is outside of the basic json spec, but *WOULD* trigger an error in parsing on invalid comment syntax
// returns `::FoundSomething` when input contained a comment, `::Nothing` otherwise
// in case of block comments, returns `::Incomplete` when unable to determine end of the comment block (aka syntax error)
template <typename TInput>
SkipCommentsResult skipComments(TInput &input) {
  auto out = SkipCommentsResult::Nothing;

  for (;;) {
    switch (input.current()) {
      default:
        goto RETURN_OUT;

      case '/':
        out = SkipCommentsResult::Incomplete;
        input.move();
        switch (input.current()) {
          default:
            goto RETURN_OUT;

          case '*':  // multi-line block comment
            input.move();
            for (;;) {  // consume '/*' and everything following, until the '*/' is encountered
              switch (input.current()) {
              case '\0':
                out = SkipCommentsResult::Incomplete;
                goto RETURN_OUT;

              case '*':
                if (input.next() == '/') {  // consumes '*/'
                  input.move();
                  input.move();
                  out = SkipCommentsResult::FoundSomething;
                  goto RETURN_OUT;
                }
                break;
              }

              input.move();
            }

            break;

           case '/':  // single-line comment
             input.move();
             for (;;) {  // consume '//' and everything following on the same line or until the end of the input
               switch (input.current()) {
               case '\0':
               case '\n':
                 out = SkipCommentsResult::FoundSomething;
                 goto RETURN_OUT;
               }

               input.move();
             }

             break;
        }
    }

    return out;
  }

RETURN_OUT:
  return out;
}

template <typename TInput>
bool skipSpacesAndComments(TInput& input) {
  for (;;) {
    const auto spaces = skipSpaces(input);
    const auto comments = skipComments(input);
    if (comments == SkipCommentsResult::Incomplete)
      return false;

    if (spaces || comments == SkipCommentsResult::FoundSomething)
      continue;

    break;
  }

  return true;
}

template <typename TInput>
bool skipBomSpacesAndComments(TInput& input) {
  const auto bom = skipBom(input);
  if (!bom)
    return false;

  return skipSpacesAndComments(input);
}

template <typename TInput>
bool skipBomSpaces(TInput& input) {
  const auto bom = skipBom(input);
  if (!bom)
    return false;

  skipSpaces(input);
  return true;
}

}  // namespace Internals
}  // namespace ArduinoJson
