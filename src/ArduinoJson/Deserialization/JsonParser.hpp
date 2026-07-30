// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../TypeTraits/IsConst.hpp"
#include "../TypeTraits/IsPointer.hpp"
#include "../TypeTraits/IsInstantiationOf.hpp"
#include "../TypeTraits/RemoveConstReference.hpp"
#include "../TypeTraits/RemovePointer.hpp"

#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/VoidType.hpp"
#include "../TypeTraits/Declval.hpp"

#include "../Data/JsonVariantContent.hpp"

#include "../JsonVariant.hpp"
#include "../JsonBuffer.hpp"
#include "../JsonSpan.hpp"
#include "../JsonString.hpp"

#include "../StringTraits/StringTraits.hpp"
#include "../Readers/Readers.hpp"

#include "JsonParserStopToken.hpp"
#include "StringWriter.hpp"
#include "StringBufferedWriter.hpp"

namespace ArduinoJson {
namespace Internals {
namespace JsonParserImpl {

template <typename T, typename = void>
struct StopTokenCallback : FalseType {
};

template <typename T>
struct StopTokenCallback<T, VoidType<decltype(
  Declval<T>()(Declval<JsonParserStopToken>(),
               Declval<const char*>(),
               Declval<JsonVariant>()))>>

  : TrueType {
};

template <typename T, typename = void>
struct BasicCallback : FalseType {
};

template <typename T>
struct BasicCallback<T, VoidType<decltype(
  Declval<T>()(Declval<const char*>(),
               Declval<JsonVariant>()))>>

  : TrueType {
};

}

// Parse JSON string to create JsonArrays and JsonObjects
// This internal class is not indended to be used directly.
// Instead, use JsonBuffer.parseArray() or .parseObject()
template <typename TReader, typename TWriter>
class JsonParser {
 public:
  using reader_type = TReader;
  using writer_type = TWriter;

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

 protected:
  static bool eat(TReader &, char charToSkip);
  ARDUINOJSON_FORCE_INLINE bool eat(char charToSkip) {
    return eat(_reader, charToSkip);
  }

  JsonString parseString();
  bool parseAnythingTo(JsonVariant *destination);

  inline bool parseArrayTo(JsonVariant *destination);
  inline bool parseObjectTo(JsonVariant *destination);
  inline bool parseStringTo(JsonVariant *destination, bool = false);

  using writer_string_type = decltype(Declval<TWriter>().startString());
  using writer_returns_json_string =
    typename IsInstantiationOf<StringBufferedWriter, writer_type>::type;

  JsonBuffer *_buffer;
  TReader _reader;
  TWriter _writer;
  uint8_t _nestingLimit;
};

template <typename TReader, typename TWriter>
class JsonKeyValueParser final :
  public JsonParser<TReader, TWriter>,
  protected JsonParserStoppable {

 public:
  using JsonParser<TReader, TWriter>::JsonParser;

  template <typename T>
  bool parseKeyValue(T&&);

 private:
  template <typename T, typename EnableIf<JsonParserImpl::StopTokenCallback<T>::value>::type* = nullptr>
  inline bool keyValueCallback(T&& callback, const char* key, JsonVariant val) {
    auto stop = makeStopToken();
    callback(stop, key, val);
    return stop.is_stopped();
  }

  template <typename T, typename EnableIf<JsonParserImpl::BasicCallback<T>::value>::type* = nullptr>
  inline bool keyValueCallback(T&& callback, const char* key, JsonVariant val) const {
    callback(key, val);
    return false;
  }

  using JsonParser<TReader, TWriter>::eat;
  using JsonParser<TReader, TWriter>::parseString;
  using JsonParser<TReader, TWriter>::parseStringTo;
  using JsonParser<TReader, TWriter>::parseAnythingTo;

  using JsonParser<TReader, TWriter>::parseObject;
  using JsonParser<TReader, TWriter>::parseArray;
  using JsonParser<TReader, TWriter>::parseVariant;
};

// internals set up 'writer' a bit differently, depending on the type of input
// generic reader, always uses TJsonBuffer as backing storage when escaped characters are encountered
template <template <typename, typename> class TJsonParser,
  typename TJsonBuffer,
  typename TJson,
  typename Enable = void>
struct JsonParserBuilder {
  typedef typename RemoveConstReference<TJson>::type TJsonNoCref;

  typedef ReaderImpl<TJsonNoCref> TReader;
  typedef StringBufferedWriter<TJsonBuffer> TWriter;
  typedef TJsonParser<TReader, TWriter> TParser;

  static TParser makeParser(TJsonBuffer *buffer, TJson &&json,
                            uint8_t nestingLimit) {
    static_assert(!IsPointer<TJsonNoCref>::value &&
                  !IsChar<typename RemovePointer<TJson>::type>::value,
                  "Avoid using T* w/ unknown size");
    return TParser(buffer, TReader(json), TWriter(*buffer), nestingLimit);
  }
};

// reuse input buffer instead of duplicating data in the TJsonBuffer. note that this causes input to be thrashed
// only enabled when string view data pointer is not marked as const
template <template <typename, typename> class TJsonParser,
  typename TJsonBuffer,
  typename TChar, size_t Size>
struct JsonParserBuilder<TJsonParser, TJsonBuffer, JsonSpan<TChar, Size>> {

  static_assert(!Internals::IsConst<TChar>::value, "");
  typedef Internals::JsonSpan<TChar, Size> TSpan;

  typedef typename StringTraits<const TChar *>::Reader TReader;
  typedef StringWriter<TChar> TWriter;

  typedef TJsonParser<TReader, TWriter> TParser;

  static TParser makeParser(TJsonBuffer *buffer, TSpan json,
                            uint8_t nestingLimit) {
    return TParser(buffer,
      TReader(json.data(), json.size()),
      TWriter(json.data(), json.size()),
      nestingLimit);
  }
};

// points to existing data when no escaped characters encountered
template <template <typename, typename> class TJsonParser,
  typename TJsonBuffer,
  typename TChar, size_t Size>
struct JsonParserBuilder<TJsonParser, TJsonBuffer, JsonSpan<const TChar, Size>> {

  typedef Internals::JsonSpan<const TChar, Size> TSpan;

  typedef typename StringTraits<TChar *>::Reader TReader;
  typedef StringBufferedWriter<TJsonBuffer> TWriter;
  typedef TJsonParser<TReader, TWriter> TParser;

  static TParser makeParser(TJsonBuffer *buffer, TSpan json, uint8_t nestingLimit) {
    return TParser(buffer,
      TReader(json.data(), json.size()),
      TWriter(*buffer),
      nestingLimit);
  }
};

template <template <typename, typename> class TJsonParser,
  typename TJsonBuffer, typename TJson>
inline typename JsonParserBuilder<TJsonParser, TJsonBuffer, TJson>::TParser
makeParser(TJsonBuffer *buffer, TJson &&json, uint8_t nestingLimit) {
  return JsonParserBuilder<TJsonParser, TJsonBuffer, TJson>::makeParser(
    buffer, std::forward<TJson>(json), nestingLimit);
}

template <template <typename, typename> class TJsonParser,
  typename TJsonBuffer, typename TChar, size_t Size>
using JsonSpanParserBuilder =
  JsonParserBuilder<TJsonParser, TJsonBuffer, Internals::JsonSpan<TChar, Size>>;

template <template <typename, typename> class TJsonParser,
  typename TJsonBuffer, typename TChar, size_t Size>
inline typename JsonSpanParserBuilder<TJsonParser, TJsonBuffer, TChar, Size>::TParser
makeParser(TJsonBuffer *buffer, TChar (&json)[Size], uint8_t nestingLimit) {
  return JsonSpanParserBuilder<TJsonParser, TJsonBuffer, TChar, Size>::makeParser(
    buffer, Internals::JsonSpan<TChar, Size>(json), nestingLimit);
}

template <template <typename, typename> class TJsonParser,
  typename TJsonBuffer, typename TChar, size_t Size>
inline typename JsonSpanParserBuilder<TJsonParser, TJsonBuffer, TChar, Size>::TParser
makeParser(TJsonBuffer *buffer, JsonStaticSpan<TChar, Size> json, uint8_t nestingLimit) {
  return JsonSpanParserBuilder<TJsonParser, TJsonBuffer, TChar, Size>::makeParser(
    buffer, json, nestingLimit);
}

template <template <typename, typename> class TJsonParser,
  typename TJsonBuffer, typename TChar>
using JsonDynamicSpanParserBuilder =
  JsonParserBuilder<TJsonParser, TJsonBuffer, JsonDynamicSpan<TChar>>;

template <template <typename, typename> class TJsonParser,
  typename TJsonBuffer, typename TChar>
inline typename JsonDynamicSpanParserBuilder<TJsonParser, TJsonBuffer, TChar>::TParser
makeParser(TJsonBuffer *buffer, JsonDynamicSpan<TChar> json, uint8_t nestingLimit) {
  return JsonDynamicSpanParserBuilder<TJsonParser, TJsonBuffer, TChar>::makeParser(
    buffer, json, nestingLimit);
}

}  // namespace Internals
}  // namespace ArduinoJson
