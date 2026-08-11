// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Comments.hpp"
#include "JsonParser.hpp"

#include "../JsonArray.hpp"
#include "../JsonObject.hpp"

#include "StringBufferedWriter.hpp"

namespace ArduinoJson {
namespace Internals {

static constexpr bool isBetween(char c, char min, char max) {
  return min <= c && c <= max;
}

static constexpr bool canBeInNonQuotedString(char c) {
  return isBetween(c, '0', '9') || isBetween(c, '_', 'z') ||
         isBetween(c, 'A', 'Z') || c == '+' || c == '-' || c == '.';
}

static constexpr bool isQuote(char c) {
  return c == '\'' || c == '\"';
}

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
    JsonVariant key;
    if (!parseStringTo(&key, true)) goto ERROR_INVALID_KEY;
    if (!eat(':')) goto ERROR_MISSING_COLON;

    // 2 - Parse value
    JsonVariant value;
    if (!parseAnythingTo(&value)) goto ERROR_INVALID_VALUE;

    // 3 - Manually search object for variant key match
    auto it = object.find_impl(
      Internals::MakeStringRef(key.as<const char*>()));
    if (it != object.end()) {  // no reason to update existing key object
      it->value = std::move(value);
    } else {  // brand new object nodes list entry
      it = object.add();
      if (it == object.end()) goto ERROR_NO_MEMORY;
      it->key = std::move(key);
      it->value = std::move(value);
    }

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
inline JsonVariant JsonParser<TReader, TWriter>::parseVariant() {
  JsonVariant result;
  parseAnythingTo(&result);
  return result;
}

template <typename TReader, typename TWriter>
inline bool JsonParser<TReader, TWriter>::parseObjectTo(
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

// JsonString *may* store data inline before falling 
// Otherwise, points to TJsonBuffer allocated storage
template <typename TReader, typename TWriter>
inline JsonString
JsonParser<TReader, TWriter>::parseString() {
  skipSpacesAndComments(_reader);
  char c = _reader.current();

  auto str = _writer.startString();

  JsonString out;

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

      if (c != '\\') // appends values as-is unless escaped
        str.append(c);
      else {
        c = _reader.current();
        if (c == '\0')
          return out;  // incomplete input

        c = Encoding::unescapeChar(c);
        if (c == '\0')
          return out;  // invalid escape

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
    return out;  // invalid
  }

  using AsJsonString = JsonParserImpl::AsJsonString<writer_returns_json_string>;
  out = AsJsonString::Operator(str);

  return out;
}

template <typename TReader, typename TWriter>
inline bool JsonParser<TReader, TWriter>::parseStringTo(
    JsonVariant *destination, bool forceString) {
  const auto hasQuotes = forceString || isQuote(_reader.current());
  const auto parsed = parseString();
  if (parsed.success()) {
    *destination = JsonVariant(parsed, hasQuotes);
    return true;
  }

  return false;
}

template <typename TReader, typename TWriter>
template <typename T>
inline bool JsonKeyValueParser<TReader, TWriter>::parseKeyValue(T&& callback) {
  if (this->_nestingLimit == 0) return false;
  this->_nestingLimit--;

  // Check opening brace
  if (!eat('{')) goto ERROR_MISSING_BRACE;
  if (eat('}')) goto SUCCESS_EMPTY_OBJECT;

  // Read each key value pair
  for (;;) {
    // 1 - Parse key
    JsonVariant key;
    if (!parseStringTo(&key, true)) goto ERROR_INVALID_KEY;
    if (!eat(':')) goto ERROR_MISSING_COLON;

    // 2 - Parse value
    JsonVariant value;
    if (!parseAnythingTo(&value)) goto ERROR_INVALID_VALUE;

    // 3 - Execute user callback and possibly stop
    if (keyValueCallback(std::forward<T>(callback), key.as<const char*>(), value)) goto SUCCESS_STOP;

    // 4 - Process more keys/values?
    if (eat('}')) goto SUCCESS_NON_EMPTY_OBJECT;
    if (!eat(',')) goto ERROR_MISSING_COMMA;
  }

SUCCESS_STOP:
SUCCESS_EMPTY_OBJECT:
SUCCESS_NON_EMPTY_OBJECT:
  this->_nestingLimit++;
  return true;

ERROR_INVALID_KEY:
ERROR_INVALID_VALUE:
ERROR_MISSING_BRACE:
ERROR_MISSING_COLON:
ERROR_MISSING_COMMA:
  return false;
}

}
}
