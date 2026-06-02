// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../JsonBuffer.hpp"
#include "../JsonVariant.hpp"
#include "../JsonSpan.hpp"

#include "../TypeTraits/IsConst.hpp"
#include "../TypeTraits/IsPointer.hpp"
#include "../TypeTraits/RemoveConstReference.hpp"
#include "../TypeTraits/RemovePointer.hpp"
#include "StringWriter.hpp"

namespace ArduinoJson {
namespace Internals {

// Parse JSON string to create JsonArrays and JsonObjects
// This internal class is not indended to be used directly.
// Instead, use JsonBuffer.parseArray() or .parseObject()
template <typename TReader, typename TWriter>
class JsonParser {
 public:
  JsonParser(JsonBuffer *buffer, TReader reader, TWriter writer,
             uint8_t nestingLimit)
      : _buffer(buffer),
        _reader(reader),
        _writer(writer),
        _nestingLimit(nestingLimit) {}

  JsonParser(const JsonParser &) = delete;
  JsonParser &operator=(const JsonParser &) = delete;

  JsonParser(JsonParser &&) = default;
  JsonParser &operator=(JsonParser &&) noexcept = default;

  JsonArray &parseArray();
  JsonObject &parseObject();

  JsonVariant parseVariant() {
    JsonVariant result;
    parseAnythingTo(&result);
    return result;
  }

 private:

  static bool eat(TReader &, char charToSkip);
  FORCE_INLINE bool eat(char charToSkip) {
    return eat(_reader, charToSkip);
  }

  const char *parseString();
  bool parseAnythingTo(JsonVariant *destination);

  inline bool parseArrayTo(JsonVariant *destination);
  inline bool parseObjectTo(JsonVariant *destination);
  inline bool parseStringTo(JsonVariant *destination);

  static inline bool isBetween(char c, char min, char max) {
    return min <= c && c <= max;
  }

  static inline bool canBeInNonQuotedString(char c) {
    return isBetween(c, '0', '9') || isBetween(c, '_', 'z') ||
           isBetween(c, 'A', 'Z') || c == '+' || c == '-' || c == '.';
  }

  static inline bool isQuote(char c) {
    return c == '\'' || c == '\"';
  }

  JsonBuffer *_buffer;
  TReader _reader;
  TWriter _writer;
  uint8_t _nestingLimit;
};

// internals set up 'writer' a bit differently, depending on the type of input
// generic reader, always uses TJsonBuffer as backing storage when escaped characters are encountered
template <typename TJsonBuffer, typename TJson, typename Enable = void>
struct JsonParserBuilder {
  typedef typename RemoveConstReference<TJson>::type TJsonNoCref;

  typedef typename StringTraits<TJsonNoCref>::Reader TReader;
  typedef JsonParser<TReader, TJsonBuffer &> TParser;

  static TParser makeParser(TJsonBuffer *buffer, TJson &&json,
                            uint8_t nestingLimit) {
    static_assert(!IsPointer<TJsonNoCref>::value &&
                  !IsChar<typename RemovePointer<TJson>::type>::value,
                  "Avoid using T* w/ unknown size");
    return TParser(buffer, TReader(json), *buffer, nestingLimit);
  }
};

// reuse input buffer instead of duplicating data in the TJsonBuffer. note that this causes input to be thrashed
// only enabled when string view data pointer is not marked as const
template <typename TJsonBuffer, typename TChar, size_t Size>
struct JsonParserBuilder<TJsonBuffer, JsonSpan<TChar, Size>> {

  static_assert(!Internals::IsConst<TChar>::value, "");
  typedef Internals::JsonSpan<TChar, Size> TSpan;

  typedef typename StringTraits<const TChar *>::Reader TReader;
  typedef StringWriter<TChar> TWriter;

  typedef JsonParser<TReader, TWriter> TParser;

  static TParser makeParser(TJsonBuffer *buffer, TSpan json,
                            uint8_t nestingLimit) {
    return TParser(buffer,
      TReader(json.data(), json.size()),
      TWriter(json.data(), json.size()),
      nestingLimit);
  }
};

// points to existing data when no escaped characters encountered
template <typename TJsonBuffer, typename TChar, size_t Size>
struct JsonParserBuilder<TJsonBuffer, JsonSpan<const TChar, Size>> {

  typedef Internals::JsonSpan<const TChar, Size> TSpan;

  typedef typename StringTraits<TChar *>::Reader TReader;
  typedef JsonParser<TReader, TJsonBuffer &> TParser;

  static TParser makeParser(TJsonBuffer *buffer, TSpan json, uint8_t nestingLimit) {
    return TParser(buffer,
      TReader(json.data(), json.size()),
      *buffer,
      nestingLimit);
  }
};

template <typename TJsonBuffer, typename TJson,
  typename TJsonNoCref = typename RemoveConstReference<TJson>::type,
  typename TBuilder = JsonParserBuilder<TJsonBuffer, TJson>>
inline typename TBuilder::TParser makeParser(
    TJsonBuffer *buffer, TJson &&json, uint8_t nestingLimit) {
  return TBuilder::makeParser(buffer, std::forward<TJson>(json), nestingLimit);
}

template <typename TJsonBuffer, typename TChar, size_t Size,
  typename TBuilder = JsonParserBuilder<TJsonBuffer, Internals::JsonSpan<TChar, Size>>>
inline typename TBuilder::TParser makeParser(
    TJsonBuffer *buffer, TChar (&json)[Size], uint8_t nestingLimit) {
  return TBuilder::makeParser(buffer, Internals::JsonSpan<TChar, Size>(json), nestingLimit);
}

template <typename TJsonBuffer, typename TChar, size_t Size,
  typename TBuilder = JsonParserBuilder<TJsonBuffer, Internals::JsonSpan<TChar, Size>>>
inline typename TBuilder::TParser makeParser(
    TJsonBuffer *buffer, JsonStaticSpan<TChar, Size> json, uint8_t nestingLimit) {
  return TBuilder::makeParser(buffer, json, nestingLimit);
}

template <typename TJsonBuffer, typename TChar,
  typename TBuilder = JsonParserBuilder<TJsonBuffer, JsonDynamicSpan<TChar>>>
inline typename TBuilder::TParser makeParser(
    TJsonBuffer *buffer, JsonDynamicSpan<TChar> json, uint8_t nestingLimit) {
  return TBuilder::makeParser(buffer, json, nestingLimit);
}

}  // namespace Internals
}  // namespace ArduinoJson
