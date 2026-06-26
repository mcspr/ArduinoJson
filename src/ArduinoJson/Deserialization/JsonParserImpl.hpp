// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Comments.hpp"
#include "JsonParser.hpp"

#include "../JsonArray.hpp"
#include "../JsonObject.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename TReader, typename TWriter>
inline bool JsonParser<TReader, TWriter>::eat(
    TReader &reader, char charToSkip) {

  skipSpacesAndComments(reader);
  const auto current = reader.current();
  if (current > 0 && current == charToSkip) {
    reader.move();
    return true;
  }
  return false;
}

template <typename TReader, typename TWriter>
inline bool JsonParser<TReader, TWriter>::parseAnythingTo(
    JsonVariant *destination) {

  skipSpacesAndComments(_reader);
  switch (_reader.current()) {
    case '[':
      return parseArrayTo(destination);

    case '{':
      return parseObjectTo(destination);
  }

  return parseStringTo(destination);
}

template <typename TReader, typename TWriter>
inline JsonArray &JsonParser<TReader, TWriter>::parseArray() {
  if (_nestingLimit == 0) return JsonArray::invalid();
  _nestingLimit--;

  // Create an empty array
  JsonArray &array = _buffer->createArray();

  // Check opening braket
  if (!eat('[')) goto ERROR_MISSING_BRACKET;
  if (eat(']')) goto SUCCESS_EMPTY_ARRAY;

  // Read each value
  for (;;) {
    // 1 - Parse value
    JsonVariant value;
    if (!parseAnythingTo(&value)) goto ERROR_INVALID_VALUE;
    if (!array.add(value)) goto ERROR_NO_MEMORY;

    // 2 - More values?
    if (eat(']')) goto SUCCES_NON_EMPTY_ARRAY;
    if (!eat(',')) goto ERROR_MISSING_COMMA;
  }

SUCCESS_EMPTY_ARRAY:
SUCCES_NON_EMPTY_ARRAY:
  _nestingLimit++;
  return array;

ERROR_INVALID_VALUE:
ERROR_MISSING_BRACKET:
ERROR_MISSING_COMMA:
ERROR_NO_MEMORY:
  return JsonArray::invalid();
}

template <typename TReader, typename TWriter>
inline bool JsonParser<TReader, TWriter>::parseArrayTo(
    JsonVariant *destination) {
  JsonArray &array = parseArray();
  if (!array.success()) return false;

  *destination = array;
  return true;
}

template <typename TReader, typename TWriter>
inline JsonObject &JsonParser<TReader, TWriter>::parseObject() {
  if (_nestingLimit == 0) return JsonObject::invalid();
  _nestingLimit--;

  // Create an empty object
  JsonObject &object = _buffer->createObject();

  // Check opening brace
  if (!eat('{')) goto ERROR_MISSING_BRACE;
  if (eat('}')) goto SUCCESS_EMPTY_OBJECT;

  // Read each key value pair
  for (;;) {
    // 1 - Parse key
    const char *key = parseString();
    if (!key) goto ERROR_INVALID_KEY;
    if (!eat(':')) goto ERROR_MISSING_COLON;

    // 2 - Parse value
    JsonVariant value;
    if (!parseAnythingTo(&value)) goto ERROR_INVALID_VALUE;
    if (!object.set(key, value)) goto ERROR_NO_MEMORY;

    // 3 - More keys/values?
    if (eat('}')) goto SUCCESS_NON_EMPTY_OBJECT;
    if (!eat(',')) goto ERROR_MISSING_COMMA;
  }

SUCCESS_EMPTY_OBJECT:
SUCCESS_NON_EMPTY_OBJECT:
  _nestingLimit++;
  return object;

ERROR_INVALID_KEY:
ERROR_INVALID_VALUE:
ERROR_MISSING_BRACE:
ERROR_MISSING_COLON:
ERROR_MISSING_COMMA:
ERROR_NO_MEMORY:
  return JsonObject::invalid();
}

template <typename TReader, typename TWriter>
inline bool JsonParser<TReader, TWriter>::parseObjectTo(
    JsonVariant *destination) {
  JsonObject &object = parseObject();
  if (!object.success()) return false;

  *destination = object;
  return true;
}

template <typename TReader, typename TWriter>
inline const char *
JsonParser<TReader, TWriter>::parseString() {
  typename RemoveReference<TWriter>::type::String str = _writer.startString();

  skipSpacesAndComments(_reader);
  char c = _reader.current();

  if (isQuote(c)) {  // quotes
    _reader.move();
    char stopChar = c;
    for (;;) {
      c = _reader.current();
      if (c == '\0')
        return nullptr;  // incomplete input
      _reader.move();

      if (c == stopChar)
        break;

      if (c != '\\') // appends values as-is unless escaped
        str.append(c);
      else {
        c = _reader.current();
        if (c == '\0')
          return nullptr;  // incomplete input

        c = Encoding::unescapeChar(c);
        if (c == '\0')
          return nullptr;  // invalid escape

        _reader.move();
        str.append(c);
      }
    }
  } else if (canBeInNonQuotedString(c)) {  // no quotes
    do {
      _reader.move();
      str.append(c);
      c = _reader.current();
    } while (canBeInNonQuotedString(c));
  } else {
    return nullptr;  // invalid
  }

  return str.c_str();
}

template <typename TReader, typename TWriter>
inline bool JsonParser<TReader, TWriter>::parseStringTo(
    JsonVariant *destination) {
  bool hasQuotes = isQuote(_reader.current());
  auto *value = parseString();
  if (value != nullptr) {
    if (hasQuotes) {
      *destination = value;
      return true;
    } else {
      *destination = RawJson(value);
      return true;
    }
  }

  return false;
}

}
}
