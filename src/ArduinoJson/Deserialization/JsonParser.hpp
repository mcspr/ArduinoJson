// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Polyfills/attributes.hpp"
#include "../Configuration.hpp"

#include "../TypeTraits/IsConst.hpp"
#include "../TypeTraits/IsPointer.hpp"
#include "../TypeTraits/IsInstantiationOf.hpp"
#include "../TypeTraits/RemoveConstReference.hpp"
#include "../TypeTraits/RemovePointer.hpp"

#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/VoidType.hpp"
#include "../TypeTraits/Declval.hpp"

#include "../Data/JsonVariantContent.hpp"
#include "../Data/Unicode.hpp"

#include "../JsonVariant.hpp"
#include "../JsonBuffer.hpp"
#include "../JsonSpan.hpp"
#include "../JsonString.hpp"

#include "../StringTraits/StringTraits.hpp"
#include "../Readers/Readers.hpp"

#include "DeserializationOptions.hpp"
#include "JsonParserStopToken.hpp"
#include "StringBufferedWriter.hpp"
#include "StringWriter.hpp"
#include "Unreadable.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename, typename, typename>
class JsonParser;

namespace JsonParserImpl {

template <typename T, typename = void>
struct StopTokenCallback : FalseType {
};

template <typename T>
struct StopTokenCallback<T, VoidType<decltype(
  Declval<T>()(Declval<JsonParserStopToken>(),
               Declval<JsonVariant>(),
               Declval<JsonVariant>()))>>

  : TrueType {
};

template <typename T, typename = void>
struct BasicCallback : FalseType {
};

template <typename T>
struct BasicCallback<T, VoidType<decltype(
  Declval<T>()(Declval<JsonVariant>(),
               Declval<JsonVariant>()))>>

  : TrueType {
};

struct NestingLimit {
  constexpr NestingLimit() = default;
  explicit constexpr NestingLimit(uint8_t limit) :
    _value(limit)
  {}

  constexpr uint8_t value() const {
    return _value;
  }

  constexpr NestingLimit take(uint8_t value_) const {
    return NestingLimit(
      _value < value_ ? static_cast<uint8_t>(_value + 1) : InvalidLimit);
  }

  constexpr NestingLimit take(const NestingLimit& other) const {
    return take(other._value);
  }

  constexpr NestingLimit release() const {
    return NestingLimit(_value && _value != InvalidLimit ? static_cast<uint8_t>(_value - 1) : _value);
  }

  constexpr explicit operator bool() const {
    return _value != InvalidLimit;
  }

  constexpr bool operator<(const NestingLimit& other) const noexcept {
    return _value < other._value;
  }

  constexpr bool operator<=(const NestingLimit& other) const noexcept {
    return _value <= other._value;
  }

 private:
  static constexpr uint8_t InvalidLimit = static_cast<uint8_t>(-1);
  uint8_t _value{ InvalidLimit };
};

struct NestingToken {
  explicit NestingToken(JsonParserImpl::NestingLimit& nesting, uint8_t nestingLimit) :
    _nesting(nesting),
    _nestingLimit(nestingLimit),
    _token(_nesting.take(_nestingLimit))
  {
    if (_token)
      _nesting = _token;
  }

  ~NestingToken() {
    if (_token)
      _nesting = _token.release();
  }

  void invalidate() {
    _token = NestingLimit(_nestingLimit);
  }

  uint8_t limit() const {
    return _token.value();
  }

  explicit operator bool() const {
    return static_cast<bool>(_token);
  }

 private:
   NestingLimit& _nesting;
   uint8_t _nestingLimit;

   NestingLimit _token;
};

struct StringContext {
  const char* stopChars = nullptr;
  bool forceString = false;

  constexpr StringContext() = default;
  constexpr StringContext(const char* stopChars_, bool forceString_) :
    stopChars(stopChars_),
    forceString(forceString_)
  {}

  static constexpr StringContext objectKey() {
    return StringContext(":}\t\n\r ", true);  // next object characters or whitespace and also disallow comments w/o whitespace
  }

  static constexpr StringContext stringLiteral(uint8_t nesting) {
    return StringContext(nesting == 0 ? "" : nullptr, false);  // skip stopChars checks when parsing arrays / objects, and only allow '\0' for plain strings
  }
};

// per spec: skip spaces between braces / brackets and strings aka values
// sort-of in spec: BOM, which is generally frowned upon, but it is adviced to simply ignore it
// (note that invalid BOM *WOULD* trigger a parsing error)
// not in spec: comments in C/C++ style that appear before or after JSON tokens
// (nb. unused funcs generally get optimized away, unless determined by runtime)
template <typename TInput>
struct SkipUnreadable {
  SkipUnreadable() = delete;

  SkipUnreadable(const SkipUnreadable&) = default;
  SkipUnreadable& operator=(const SkipUnreadable&) = default;

  SkipUnreadable(SkipUnreadable&&) = default;
  SkipUnreadable& operator=(SkipUnreadable&&) = default;

  constexpr explicit SkipUnreadable(DeserializationOptions options) :
    _skipUnreadable(
      options.skipBom && options.enableComments ?
        &SkipUnreadable::_skipBomSpacesAndComments :
      options.skipBom ?
        &SkipUnreadable::_skipBomSpaces :
      options.enableComments ?
        &SkipUnreadable::_skipSpacesAndComments :
        &SkipUnreadable::_skipSpaces)
  {}

  bool _skipBomSpacesAndComments(TInput &input) {
   if (!skipBomSpacesAndComments(input))
     return false;

   _skipUnreadable = &SkipUnreadable::_skipSpacesAndComments;
   return true;
  }

  bool _skipSpacesAndComments(TInput &input) {
    return skipSpacesAndComments(input);
  }

  bool _skipBomSpaces(TInput &input) {
    if (!skipBomSpaces(input))
      return false;

    _skipUnreadable = &SkipUnreadable::_skipSpaces;
    return true;
  }

  bool _skipSpaces(TInput &input) {
    skipSpaces(input);
    return true;
  }

  bool skipUnreadable(TInput& input) {
    return (this->*_skipUnreadable)(input);
  }

 private:
  using skip_type = bool(SkipUnreadable::*)(TInput&);
  skip_type _skipUnreadable;
};

// abi helper for unicode configuration options, binding TState & TCodepoint to the parser type
// so the compiler generates different method signatures for different configuration options
template <typename TState, typename TCodepoint>
struct ParseString {
  using state_type = TState;
  using codepoint_type = TCodepoint;
};

// nb. aliases are not advertised in the binary, just the class / struct above
using ParseStringImpl = ParseString<
    Unicode::Utf8::State,
    Unicode::Utf16::Codepoint>;

}  // namespace JsonParserImpl

// Parse JSON string to create JsonArrays and JsonObjects
// This internal class is not indended to be used directly.
// Instead, use JsonBuffer.parseArray() or .parseObject()
template <typename TReader, typename TWriter, typename TParseString>
class JsonParser {
 public:
  using reader_type = TReader;
  using writer_type = TWriter;

  JsonParser(JsonBuffer *buffer, TReader reader, TWriter writer, DeserializationOptions options) :
    _buffer(buffer),
    _reader(reader),
    _writer(writer),
    _nesting(0),
    _nestingLimit(options.nestingLimit),
    _skipUnreadable(options)
  {}

  JsonParser(const JsonParser &) = delete;
  JsonParser &operator=(const JsonParser &) = delete;

  JsonParser(JsonParser &&) = default;
  JsonParser &operator=(JsonParser &&) noexcept = default;

  JsonArray &parseArray();
  JsonObject &parseObject();

  JsonVariant parseVariant();

 protected:
  char eat(char charToSkip);
  inline bool eatExact(char charToSkip) {
    return eat(charToSkip) == charToSkip;
  }

  bool skipUnreadable();

  using writer_string_type = decltype(Declval<TWriter>().startString());
  using writer_returns_json_string =
    typename IsInstantiationOf<StringBufferedWriter, writer_type>::type;

  JsonString parseString(const char* stopChars);
  bool parseAnythingTo(JsonVariant *destination);

  bool parseArrayTo(JsonVariant *destination);
  bool parseObjectTo(JsonVariant *destination);

  using StringContext = JsonParserImpl::StringContext;

  bool parseStringTo(JsonVariant *destination, StringContext);
  bool parseStringTo(JsonVariant *destination);

  bool parseObjectKeyTo(JsonVariant *destination);

  using NestingToken = JsonParserImpl::NestingToken;

  NestingToken makeNestingToken() {
    return NestingToken(_nesting, _nestingLimit.value());
  }

  JsonBuffer *_buffer;
  TReader _reader;
  TWriter _writer;

  using NestingLimit = JsonParserImpl::NestingLimit;

  NestingLimit _nesting;
  NestingLimit _nestingLimit;

  using SkipUnreadable = JsonParserImpl::SkipUnreadable<TReader>;

  SkipUnreadable _skipUnreadable;
};

template <typename TReader, typename TWriter, typename TParseString>
class JsonKeyValueParser final :
  public JsonParser<TReader, TWriter, TParseString>,
  protected JsonParserStoppable {

  using Base = JsonParser<TReader, TWriter, TParseString>;

 public:
  using Base::JsonParser;

  template <typename T>
  bool parseKeyValue(T&&);

 private:
  template <typename T, typename EnableIf<JsonParserImpl::StopTokenCallback<T>::value>::type* = nullptr>
  inline bool keyValueCallback(T&& callback, JsonVariant key, JsonVariant val) {
    auto stop = makeStopToken();
    callback(stop, key, val);
    return stop.is_stopped();
  }

  template <typename T, typename EnableIf<JsonParserImpl::BasicCallback<T>::value>::type* = nullptr>
  inline bool keyValueCallback(T&& callback, JsonVariant key, JsonVariant val) const {
    callback(key, val);
    return false;
  }

  using Base::eat;
  using Base::eatExact;

  using Base::parseAnythingTo;
  using Base::parseString;
  using Base::parseStringTo;
  using Base::parseObjectKeyTo;

  using Base::makeNestingToken;

  using Base::parseArray;
  using Base::parseObject;
  using Base::parseVariant;
};

// internals set up 'writer' a bit differently, depending on the type of input
// generic reader, always uses TJsonBuffer as backing storage when escaped characters are encountered
template <template <typename, typename, typename> class TJsonParser,
  typename TParseString,
  typename TJsonBuffer,
  typename TJson,
  typename Enable = void>
struct JsonParserBuilder {
  typedef typename RemoveConstReference<TJson>::type TJsonNoCref;

  typedef ReaderImpl<TJsonNoCref> TReader;
  typedef StringBufferedWriter<TJsonBuffer> TWriter;
  typedef TJsonParser<TReader, TWriter, TParseString> TParser;

  static TParser makeParser(TJsonBuffer *buffer, TJson &&json,
                            DeserializationOptions deserializationOptions) {
    static_assert(!IsPointer<TJsonNoCref>::value &&
                  !IsChar<typename RemovePointer<TJson>::type>::value,
                  "Avoid using T* w/ unknown size");
    return TParser(buffer, TReader(json), TWriter(*buffer), deserializationOptions);
  }
};

// reuse input buffer instead of duplicating data in the TJsonBuffer. note that this causes input to be thrashed
// only enabled when string view data pointer is not marked as const
template <template <typename, typename, typename> class TJsonParser,
  typename TParseString,
  typename TJsonBuffer,
  typename TChar, size_t Size>
struct JsonParserBuilder<TJsonParser, TParseString, TJsonBuffer, JsonSpan<TChar, Size>> {

  static_assert(!Internals::IsConst<TChar>::value, "");
  typedef Internals::JsonSpan<TChar, Size> TSpan;

  typedef typename StringTraits<const TChar *>::Reader TReader;
  typedef StringWriter<TChar> TWriter;

  typedef TJsonParser<TReader, TWriter, TParseString> TParser;

  static TParser makeParser(TJsonBuffer *buffer, TSpan json,
                            DeserializationOptions deserializationOptions) {
    return TParser(buffer,
      TReader(json.data(), json.size()),
      TWriter(json.data(), json.size()),
      deserializationOptions);
  }
};

// points to existing data when no escaped characters encountered
template <template <typename, typename, typename> class TJsonParser,
  typename TParseString,
  typename TJsonBuffer,
  typename TChar, size_t Size>
struct JsonParserBuilder<TJsonParser, TParseString, TJsonBuffer, JsonSpan<const TChar, Size>> {

  typedef Internals::JsonSpan<const TChar, Size> TSpan;

  typedef typename StringTraits<TChar *>::Reader TReader;
  typedef StringBufferedWriter<TJsonBuffer> TWriter;
  typedef TJsonParser<TReader, TWriter, TParseString> TParser;

  static TParser makeParser(TJsonBuffer *buffer, TSpan json,
                            DeserializationOptions deserializationOptions) {
    return TParser(buffer,
      TReader(json.data(), json.size()),
      TWriter(*buffer),
      deserializationOptions);
  }
};

template <template <typename, typename, typename> class TJsonParser,
  typename TParseString, typename TJsonBuffer, typename TJson>
inline typename JsonParserBuilder<TJsonParser, TParseString, TJsonBuffer, TJson>::TParser
makeParser(TJsonBuffer *buffer, TJson &&json, DeserializationOptions deserializationOptions) {
  return JsonParserBuilder<TJsonParser, TParseString, TJsonBuffer, TJson>::makeParser(
    buffer, std::forward<TJson>(json), deserializationOptions);
}

template <template <typename, typename, typename> class TJsonParser,
  typename TParseString, typename TJsonBuffer, typename TChar, size_t Size>
using JsonSpanParserBuilder =
  JsonParserBuilder<TJsonParser, TParseString, TJsonBuffer, Internals::JsonSpan<TChar, Size>>;

template <template <typename, typename, typename> class TJsonParser,
  typename TParseString, typename TJsonBuffer, typename TChar, size_t Size>
inline typename JsonSpanParserBuilder<TJsonParser, TParseString, TJsonBuffer, TChar, Size>::TParser
makeParser(TJsonBuffer *buffer, TChar (&json)[Size], DeserializationOptions deserializationOptions) {
  return JsonSpanParserBuilder<TJsonParser, TParseString, TJsonBuffer, TChar, Size>::makeParser(
    buffer, Internals::JsonSpan<TChar, Size>(json), deserializationOptions);
}

template <template <typename, typename, typename> class TJsonParser,
  typename TParseString, typename TJsonBuffer, typename TChar, size_t Size>
inline typename JsonSpanParserBuilder<TJsonParser, TParseString, TJsonBuffer, TChar, Size>::TParser
makeParser(TJsonBuffer *buffer, JsonStaticSpan<TChar, Size> json, DeserializationOptions deserializationOptions) {
  return JsonSpanParserBuilder<TJsonParser, TParseString, TJsonBuffer, TChar, Size>::makeParser(
    buffer, json, deserializationOptions);
}

template <template <typename, typename, typename> class TJsonParser,
  typename TParseString, typename TJsonBuffer, typename TChar>
using JsonDynamicSpanParserBuilder =
  JsonParserBuilder<TJsonParser, TParseString, TJsonBuffer, JsonDynamicSpan<TChar>>;

template <template <typename, typename, typename> class TJsonParser,
  typename TParseString, typename TJsonBuffer, typename TChar>
inline typename JsonDynamicSpanParserBuilder<TJsonParser, TParseString, TJsonBuffer, TChar>::TParser
makeParser(TJsonBuffer *buffer, JsonDynamicSpan<TChar> json, DeserializationOptions deserializationOptions) {
  return JsonDynamicSpanParserBuilder<TJsonParser, TParseString, TJsonBuffer, TChar>::makeParser(
    buffer, json, deserializationOptions);
}

}  // namespace Internals
}  // namespace ArduinoJson
