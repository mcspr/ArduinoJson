// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../TypeTraits/AddConst.hpp"
#include "../TypeTraits/Conditional.hpp"
#include "../TypeTraits/IsInstantiationOf.hpp"
#include "../TypeTraits/RemoveConstReference.hpp"

#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/VoidType.hpp"
#include "../TypeTraits/Declval.hpp"

#include "../Data/JsonVariantContent.hpp"
#include "../Data/StringViewRef.hpp"
#include "../Data/Unicode.hpp"

#include "../JsonVariant.hpp"
#include "../JsonBuffer.hpp"
#include "../JsonSpan.hpp"
#include "../JsonString.hpp"

#include "../StringTraits/StringTraits.hpp"
#include "../Readers/Readers.hpp"

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

  NestingLimit(const NestingLimit&) = default;
  NestingLimit(NestingLimit&&) = default;

  NestingLimit& operator=(const NestingLimit&) = default;
  NestingLimit& operator=(NestingLimit&&) = default;

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
    return StringContext(":\t\n\r ", true);  // next object characters or whitespace and also disallow comments w/o whitespace
  }

  static constexpr StringContext stringLiteral(uint8_t nesting) {
    return StringContext(nesting == 0 ? "" : nullptr, false);  // skip stopChars checks when parsing arrays / objects, and only allow '\0' for plain strings
  }
};

// per spec: skip spaces between braces / brackets and strings aka values
// sort-of in spec: BOM, which is generally frowned upon, but it is advised to simply ignore it
// (note that invalid BOM *WOULD* trigger a parsing error)
// not in spec: single-line and multi-line comments that appear before or after JSON tokens
// (nb. unused funcs generally get optimized away, but this heavily depends on whether the compiler actually switches into constexpr context)
template <typename TInput>
struct SkipUnreadable {
  SkipUnreadable() = delete;

  SkipUnreadable(const SkipUnreadable&) = default;
  SkipUnreadable& operator=(const SkipUnreadable&) = default;

  SkipUnreadable(SkipUnreadable&&) = default;
  SkipUnreadable& operator=(SkipUnreadable&&) = default;

  constexpr explicit SkipUnreadable(bool skipBom, bool enableComments) :
    _skipUnreadable(
      skipBom && enableComments ?
        &SkipUnreadable::_skipBomSpacesAndComments :
      skipBom ?
        &SkipUnreadable::_skipBomSpaces :
      enableComments ?
        &SkipUnreadable::_skipSpacesAndComments :
        &SkipUnreadable::_skipSpaces)
  {}

  bool skipUnreadable(TInput& input) {
    return (this->*_skipUnreadable)(input);
  }

 private:
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
    return skipSpaces(input) || true;
  }

  using skip_type = bool(SkipUnreadable::*)(TInput&);
  skip_type _skipUnreadable;
};

// abi helper for unicode configuration options, binding TState & TCodepoint to the parser type
// so the compiler generates different method identity for different configuration options
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

  using nesting_token_type = JsonParserImpl::NestingToken;
  using nesting_limit_type = JsonParserImpl::NestingLimit;

  using skip_unreadable_type = JsonParserImpl::SkipUnreadable<TReader>;

  JsonParser(JsonBuffer *buffer, TReader reader, TWriter writer,
    nesting_limit_type nestingLimit_,
    skip_unreadable_type skipUnreadable_) :

    _buffer(buffer),
    _reader(std::move(reader)),
    _writer(std::move(writer)),
    _nesting(0),
    _nestingLimit(nestingLimit_),
    _skipUnreadable(skipUnreadable_)
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

  using string_context_type = JsonParserImpl::StringContext;

  bool parseStringTo(JsonVariant *destination, string_context_type);
  bool parseStringTo(JsonVariant *destination);

  bool parseObjectKeyTo(JsonVariant *destination);

  nesting_token_type makeNestingToken() {
    return nesting_token_type(_nesting, _nestingLimit.value());
  }

  JsonBuffer *_buffer;
  TReader _reader;
  TWriter _writer;

  nesting_limit_type _nesting;
  nesting_limit_type _nestingLimit;

  skip_unreadable_type _skipUnreadable;
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

template <typename TJson, typename = void>
struct JsonParserInput {
  using type = TJson;
};

template <typename TJson>
struct JsonParserInput<TJson,
    typename EnableIf<HasStringTraits<TJson>::value>::type> {

  using type =
    typename Internals::StringRefWrapperHelper<TJson>::wrapper_type;
};

template <typename TChar, size_t Size>
struct JsonParserInput<JsonSpan<TChar, Size>> {
  using type =
    typename Internals::StringRefWrapper<SizedStringView<TChar>>;
};

// generic reader, deduced through the reader traits and used by default if nothing else matches
// this usually means it is some kind of special object or stream, since string-like types prefer specializations below
template <typename TJson, typename = void>
struct JsonParserReader {
  using reader_type = typename ReaderImpl<
    typename RemoveConstReference<TJson>::type>::type;

  static reader_type make(TJson&& json) {
    return reader_type(std::forward<TJson>(json));
  }
};
 
// Fixed size view, usually generated through inline string literals or explicit char array references
template <typename TChar>
struct JsonParserReader<Internals::StringRefWrapper<SizedStringView<TChar>>> {
  using json_type = Internals::SizedStringView<TChar>;
  using reader_type = typename StringTraits<
    typename AddConst<TChar>::type *>::Reader;

  static reader_type make(json_type json) {
    return reader_type(json.data(), json.size());
  }
};

// Unknown size view, ignore
template <typename TChar>
struct JsonParserReader<Internals::StringRefWrapper<UnsizedStringView<TChar>>> {
  static_assert(!IsChar<TChar>::value, "Avoid using T* w/ unknown size");
};

// Anything else that was previously deduced to have StringTraits present, but should not be reduced to a string view
template <typename TJson>
struct JsonParserReader<Internals::StringRefWrapper<TJson>> {
  using json_type = TJson;

  // nb. avoid dangling reference (oops) from within string ref for ephemeral objects
  // JsonParser & TJson&& lifetime usually ends within JsonBufferBase::parse... methods
  // Since JsonParser holds the reader instance, make sure ref survives through it
  struct reader_type :
      Internals::StringRefWrapper<TJson>,
      StringTraits<TJson>::Reader {

   explicit reader_type(Internals::StringRefWrapper<TJson> str) :
     Internals::StringRefWrapper<TJson>(std::move(str)),
     StringTraits<TJson>::Reader(Internals::StringRefWrapper<TJson>::get())
   {}

   reader_type(reader_type&& other) noexcept :
     Internals::StringRefWrapper<TJson>(std::move(other)),
     StringTraits<TJson>::Reader(Internals::StringRefWrapper<TJson>::get())
   {}
  };

  static reader_type make(Internals::StringRefWrapper<TJson> json) {
    return reader_type(std::move(json));
  }
};

// internals set up 'writer' a bit differently, depending on the type of input
// by default, duplicates read data in the provided TJsonBuffer
struct JsonParserWriterBufferTag {
};

template <typename TJsonBuffer, typename TJson, typename = void>
struct JsonParserWriter : JsonParserWriterBufferTag {
  using json_type = TJson;
  using writer_type = StringBufferedWriter<TJsonBuffer>;

  static writer_type make(TJsonBuffer* buffer) {
    return writer_type(*buffer);
  }
};

// reuse input buffer instead of duplicating data in the TJsonBuffer. note that this causes input to be thrashed
// only enabled when data span is not marked as const
struct JsonParserWriterInputTag {
};

template <typename TJsonBuffer, typename TChar>
struct JsonParserWriter<TJsonBuffer,
    StringRefWrapper<SizedStringView<TChar>>> : JsonParserWriterInputTag {

  using json_type = SizedStringView<TChar>;
  using writer_type = StringWriter<TChar>;

  static StringWriter<TChar> make(SizedStringView<TChar> json) {
    return StringWriter<TChar>(json.data(), json.size());
  }
};

// points to existing data when no escaped characters encountered
template <typename TJsonBuffer, typename TChar>
struct JsonParserWriter<TJsonBuffer,
    StringRefWrapper<SizedStringView<const TChar>>> : JsonParserWriterBufferTag {

  using json_type = SizedStringView<const TChar>;
  using writer_type = StringBufferedWriter<TJsonBuffer>;

  static writer_type make(TJsonBuffer* buffer) {
    return writer_type(*buffer);
  }
};

template <template <typename, typename, typename> class TJsonParser,
  typename TParseString, typename TJsonBuffer, typename TJson>
struct JsonParserBuilder {
  using json_type = typename JsonParserInput<TJson>::type;

  using parser_reader_type = JsonParserReader<json_type>;
  using parser_writer_type = JsonParserWriter<TJsonBuffer, json_type>;
  using parser_type = TJsonParser<
    typename parser_reader_type::reader_type,
    typename parser_writer_type::writer_type,
    TParseString>;

  using nesting_limit_type = JsonParserImpl::NestingLimit;
  using skip_unreadable_type =
    JsonParserImpl::SkipUnreadable<typename parser_reader_type::reader_type>;

  static parser_type
  makeParser(TJsonBuffer* buffer, TJson&& json,
    nesting_limit_type nestingLimit,
    skip_unreadable_type skipUnreadable)
  {
    return make_parser::Operator(
      buffer, json_type(std::forward<TJson>(json)),
      nestingLimit, skipUnreadable);
  }

 private:
  struct make_parser_with_buffer_writer {
    static parser_type
    Operator(TJsonBuffer* buffer, json_type&& json,
      nesting_limit_type nestingLimit,
      skip_unreadable_type skipUnreadable)
    {
      return parser_type(buffer,
        parser_reader_type::make(std::forward<json_type>(json)),
        parser_writer_type::make(buffer),
        nestingLimit, skipUnreadable);
    }
  };

  struct make_parser_with_json_writer {
    static parser_type
    Operator(TJsonBuffer* buffer, json_type&& json,
      nesting_limit_type nestingLimit,
      skip_unreadable_type skipUnreadable)
    {
      return parser_type(buffer,
        parser_reader_type::make(json),
        parser_writer_type::make(json),
        nestingLimit, skipUnreadable);
    }
  };

  static constexpr bool is_buffer_writer =
    IsBaseOf<JsonParserWriterBufferTag, parser_writer_type>::type::value;

  using make_parser = Conditional<
    is_buffer_writer,
    make_parser_with_buffer_writer,
    make_parser_with_json_writer>;
};

template <template <typename, typename, typename> class TJsonParser, typename TParseString,
    typename TDeserializationOptions, typename TJsonBuffer, typename TJson>
constexpr inline typename JsonParserBuilder<TJsonParser, TParseString, TJsonBuffer, TJson>::parser_type
makeParser(TJsonBuffer* buffer, TJson&& json, TDeserializationOptions deserializationOptions) {
  using builder_type = JsonParserBuilder<
    TJsonParser, TParseString,
    TJsonBuffer, TJson>;

  return builder_type::makeParser(
    buffer, std::forward<TJson>(json),
    typename builder_type::nesting_limit_type(
      deserializationOptions.nestingLimit),
    typename builder_type::skip_unreadable_type(
      deserializationOptions.skipBom,
      deserializationOptions.enableComments));
}

}  // namespace Internals
}  // namespace ArduinoJson
