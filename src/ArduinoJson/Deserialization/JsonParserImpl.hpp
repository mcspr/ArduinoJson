// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonParser.hpp"

#include "../JsonArray.hpp"
#include "../JsonObject.hpp"

#include "../Data/Character.hpp"
#include "../Data/Unicode.hpp"

#include "StringBufferedWriter.hpp"

namespace ArduinoJson {
namespace Internals {
namespace JsonParserImpl {

struct Codeunit {
  Character::Deserialization::Nibbles lhs;
  Character::Deserialization::Nibbles rhs;

  constexpr explicit Codeunit(const char (&nibbles)[4]) :
    lhs(Character::Deserialization::fromNibbles(nibbles[0], nibbles[1])),
    rhs(Character::Deserialization::fromNibbles(nibbles[2], nibbles[3]))
  {}

  constexpr bool ok() const {  // currently no-op as parser checks this before construction
    return lhs.ok() && rhs.ok();
  }

  constexpr uint16_t value() const {
    return static_cast<uint16_t>(
      static_cast<uint16_t>(static_cast<uint16_t>(lhs.value()) << 8) |
      static_cast<uint16_t>(rhs.value()));
  }
};

}

template <typename TReader, typename TWriter, typename TParseString>
inline bool JsonParser<TReader, TWriter, TParseString>::skipUnreadable()
{
  return _skipUnreadable.skipUnreadable(_reader);
}

template <typename TReader, typename TWriter, typename TParseString>
inline char JsonParser<TReader, TWriter, TParseString>::eat(char charToSkip) {
  if (!skipUnreadable())
    return '\0';

  const auto current = _reader.current();
  if (current > 0 && current == charToSkip) {
    _reader.move();
  }

  return current;
}

template <typename TReader, typename TWriter, typename TParseString>
inline bool JsonParser<TReader, TWriter, TParseString>::parseAnythingTo(
    JsonVariant *destination) {

  if (!skipUnreadable())
    return false;

  switch (_reader.current()) {
    case '[':
      return parseArrayTo(destination);

    case '{':
      return parseObjectTo(destination);
  }

  return parseStringTo(destination,
      StringContext::stringLiteral(_nesting.value()));
}

template <typename TReader, typename TWriter, typename TParseString>
inline JsonArray& JsonParser<TReader, TWriter, TParseString>::parseArray() {
  auto nesting = makeNestingToken();
  if (!nesting)
    return JsonArray::invalid();

  // Create an empty array
  JsonArray &array = _buffer->createArray();

  // Check opening braket
  if (!eatExact('['))
    goto ERROR_MISSING_BRACKET;

  switch (eat(']')) {
  case '\0':
    goto ERROR_MISSING_BRACKET;

  case ']':
    goto SUCCESS_EMPTY_ARRAY;
  }

  // Read each value
  for (;;) {
    // 1 - Parse value
    JsonVariant value;
    if (!parseAnythingTo(&value))
      goto ERROR_INVALID_VALUE;
    if (!array.add(value))
      goto ERROR_NO_MEMORY;

    // 2 - More values?
    switch (eat(']')) {
    case '\0':
      goto ERROR_MISSING_BRACKET;

    case ']':
      goto SUCCESS_NON_EMPTY_ARRAY;
    }

    if (!eatExact(','))
      goto ERROR_MISSING_COMMA;
  }

SUCCESS_EMPTY_ARRAY:
SUCCESS_NON_EMPTY_ARRAY:
  return array;

ERROR_INVALID_VALUE:
ERROR_MISSING_BRACKET:
ERROR_MISSING_COMMA:
ERROR_NO_MEMORY:
  nesting.invalidate();
  return JsonArray::invalid();
}

template <typename TReader, typename TWriter, typename TParseString>
inline bool JsonParser<TReader, TWriter, TParseString>::parseArrayTo(
    JsonVariant *destination) {
  JsonArray &array = parseArray();
  if (!array.success()) return false;

  *destination = array;
  return true;
}

template <typename TReader, typename TWriter, typename TParseString>
inline JsonObject &JsonParser<TReader, TWriter, TParseString>::parseObject() {
  auto nesting = makeNestingToken();
  if (!nesting)
    return JsonObject::invalid();

  // Create an empty object
  JsonObject &object = _buffer->createObject();

  // Check opening brace
  if (!eatExact('{'))
    goto ERROR_MISSING_BRACE;

  switch (eat('}')) {
    case '\0':
      goto ERROR_MISSING_BRACE;

    case '}':
      goto SUCCESS_EMPTY_OBJECT;
  }

  // Read each key value pair
  for (;;) {
    // 1 - Parse key
    JsonVariant key;
    if (!parseObjectKeyTo(&key))
      goto ERROR_INVALID_KEY;
    if (!eatExact(':'))
      goto ERROR_MISSING_COLON;

    // 2 - Parse value
    JsonVariant value;
    if (!parseAnythingTo(&value))
      goto ERROR_INVALID_VALUE;

    // 3 - Manually search object for variant key match
    auto it = object.find_impl(
      Internals::MakeStringRef(key.as<const char*>()));
    if (it != object.end()) {  // no reason to update existing key object
      it->value = std::move(value);
    } else {  // brand new object nodes list entry
      it = object.add();
      if (it == object.end())
        goto ERROR_NO_MEMORY;
      it->key = std::move(key);
      it->value = std::move(value);
    }

    // 3 - More keys/values?
    switch (eat('}')) {
      case '\0':
        goto ERROR_MISSING_BRACE;

      case '}':
        goto SUCCESS_NON_EMPTY_OBJECT;
    }

    if (!eatExact(','))
      goto ERROR_MISSING_COMMA;
  }

SUCCESS_EMPTY_OBJECT:
SUCCESS_NON_EMPTY_OBJECT:
  return object;

ERROR_INVALID_KEY:
ERROR_INVALID_VALUE:
ERROR_MISSING_BRACE:
ERROR_MISSING_COLON:
ERROR_MISSING_COMMA:
ERROR_NO_MEMORY:
  nesting.invalidate();
  return JsonObject::invalid();
}

template <typename TReader, typename TWriter, typename TParseString>
inline JsonVariant JsonParser<TReader, TWriter, TParseString>::parseVariant() {
  JsonVariant result;
  parseAnythingTo(&result);
  return result;
}

template <typename TReader, typename TWriter, typename TParseString>
inline bool JsonParser<TReader, TWriter, TParseString>::parseObjectTo(
    JsonVariant *destination) {
  JsonObject &object = parseObject();
  if (!object.success()) return false;

  *destination = object;
  return true;
}

namespace JsonParserImpl {

template <typename Convertible>
struct AsJsonString;

template <>
struct AsJsonString<TrueType> {
  template <typename TString>
  static JsonString Operator(TString& str) {
    return str.asJsonString();
  }
};

template <>
struct AsJsonString<FalseType> {
  template <typename TString>
  static JsonString Operator(TString& str) {
    return JsonString(str.c_str());
  }
};

}

// conditionally called based on the probing done in the parser
// appends to the internal buffer until full, then the parent buffer
template <typename TWriter>
inline void StringBufferedWriter<TWriter>::String::_appendValue(
  string_type& str, char c)
{
  auto& capacity = last();
  const auto windex = LastIndex - static_cast<size_t>(capacity);

  if (capacity == '\0') {
    for (size_t index = 0; index < windex; ++index)
      str.append(_buffer.value[index]);
    str.append(c);
    _append = &String::_appendParent;
    return;
  }

  _buffer.value[windex] = c;
  --capacity;
}

// either forced or switched to from the internal buffer
template <typename TWriter>
inline void StringBufferedWriter<TWriter>::String::_appendParent(
  string_type& str, char c)
{
  str.append(c);
}

// JsonString *may* store data inline instead of the TJsonBuffer
// (i.e. returned pointer may be ephemeral and does not always point to TJsonBuffer allocated storage)
template <typename TReader, typename TWriter, typename TParseString>
inline JsonString
JsonParser<TReader, TWriter, TParseString>::parseString(const char* stopChars) {

  JsonString out;
  if (!skipUnreadable())
    return out;  // cannot read

  char c = _reader.current();

  auto str = _writer.startString();

  // similar to 7.x, decode & re-encode utf16 codepoints from escaped byte sequences
  // note that both codepoints and normal chars go through the utf8 state (unless disabled through configuration)
  typename TParseString::codepoint_type codepoint;
  typename TParseString::state_type state;

  if (isQuote(c)) {  // quotes
    _reader.move();
    char stopChar = c;
    for (;;) {
      c = _reader.current();
      if (c == '\0')
        return out;  // incomplete input
      _reader.move();

      if (c == stopChar)
        break;

      if (!canBeInQuotedString(c))
        return out;  // invalid character range

      if (c != '\\') {  // appends characters as-is unless escaped
        if (!state.append(c))
          return out;  // invalid character state

        str.append(c);
      } else {
        c = _reader.current();
        if (c == '\0')
          return out;  // incomplete input

        if (c == 'u') {
          _reader.move();

          char tmp[4];
          for (size_t n = 0; n < sizeof(tmp); ++n) {
            tmp[n] = _reader.current();
            if (tmp[n] == '\0')
              return out;  // incomplete input

            if (!Character::Deserialization::isNibble(tmp[n]))
              return out;  // invalid hexadecimal input

            _reader.move();
          }

          auto codeunit = JsonParserImpl::Codeunit(tmp);
          if (!codeunit.ok())
            return out;  // invalid input

          if (!codeunit.value())
            return out;  // embedded null

          if (codepoint.append(codeunit.value())) {
            auto bytes = Unicode::Utf8::encode(codepoint.value());
            for (auto b : bytes.value) {
              c = static_cast<char>(b);
              if (!c)
                continue;  // leading zeroes are ok here

              if (!state.append(c))
                return out;  // invalid character state

              str.append(c);
            }
          }

        } else {
          c = Character::Backslash::unescapeChar(c);
          if (c == '\0')
            return out;  // invalid escape

          _reader.move();
          str.append(c);
        }
      }
    }
  } else if (canBeInNonQuotedString(c)) {  // no quotes
    do {
      _reader.move();
      str.append(c);
      c = _reader.current();
    } while (canBeInNonQuotedString(c));  // note that only ascii range is allowed

    if (stopChars) {  // and actual stopping point depends on where the string is
      char end = '\0';
      do {
        end = *(stopChars++);
        if (end == c)
          goto RETURN_JSON_STRING;
      } while (end != '\0');

      if (end == '\0') {
        return out;  // invalid
      }
    }
  } else {
    return out;  // invalid
  }

RETURN_JSON_STRING:
  if (state) {
    using AsJsonString = JsonParserImpl::AsJsonString<writer_returns_json_string>;
    out = AsJsonString::Operator(str);
  }

  return out;
}

template <typename TReader, typename TWriter, typename TParseString>
inline bool JsonParser<TReader, TWriter, TParseString>::parseStringTo(
    JsonVariant *destination, StringContext context) {

  const auto hasQuotes = context.forceString || isQuote(_reader.current());
  const auto parsed = parseString(context.stopChars);
  if (parsed.success()) {
    *destination = JsonVariant(parsed, hasQuotes);
    return true;
  }

  return false;
}

template <typename TReader, typename TWriter, typename TParseString>
inline bool JsonParser<TReader, TWriter, TParseString>::parseStringTo(JsonVariant *destination) {
  return parseStringTo(destination, StringContext());
}

template <typename TReader, typename TWriter, typename TParseString>
inline bool JsonParser<TReader, TWriter, TParseString>::parseObjectKeyTo(
    JsonVariant *destination) {
  return parseStringTo(destination, StringContext::objectKey());
}

template <typename TReader, typename TWriter, typename TParseString>
template <typename T>
inline bool JsonKeyValueParser<TReader, TWriter, TParseString>::parseKeyValue(T&& callback) {
  auto nesting = makeNestingToken();
  if (!nesting)
    return false;

  // Check opening brace
  if (!eatExact('{'))
    goto ERROR_MISSING_BRACE;

  switch (eat('}')) {
  case '\0':
    goto ERROR_MISSING_BRACE;

  case '}':
    goto SUCCESS_EMPTY_OBJECT;
  }

  // Read each key value pair
  for (;;) {
    // 1 - Parse key
    JsonVariant key;
    if (!parseObjectKeyTo(&key))
      goto ERROR_INVALID_KEY;
    if (!eat(':'))
      goto ERROR_MISSING_COLON;

    // 2 - Parse value
    JsonVariant value;
    if (!parseAnythingTo(&value))
      goto ERROR_INVALID_VALUE;

    // 3 - Execute user callback and possibly stop
    if (keyValueCallback(std::forward<T>(callback), key, value))
      goto SUCCESS_STOP;

    // 4 - Process more keys/values?
    switch (eat('}')) {
      case '\0':
        goto ERROR_MISSING_BRACE;

      case '}':
        goto SUCCESS_NON_EMPTY_OBJECT;
    }

    if (!eatExact(','))
      goto ERROR_MISSING_COMMA;
  }

SUCCESS_STOP:
SUCCESS_EMPTY_OBJECT:
SUCCESS_NON_EMPTY_OBJECT:
  return true;

ERROR_INVALID_KEY:
ERROR_INVALID_VALUE:
ERROR_MISSING_BRACE:
ERROR_MISSING_COLON:
ERROR_MISSING_COMMA:
  nesting.invalidate();
  return false;
}

}
}
