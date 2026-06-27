// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Strings/Strings.hpp"
#include "../Data/Encoding.hpp"
#include "../Data/JsonInteger.hpp"
#include "../Data/JsonFloat.hpp"
#include "../Serialization/FloatParts.hpp"
#include "../Polyfills/math.hpp"

#include <cstdint>
#include <limits>

namespace ArduinoJson {
namespace Internals {
namespace JsonNumberWriter {

// writer is expecting a fixed type unsigned value regardless of the original type
static constexpr auto Base10UIntDigits =
    size_t{ 3 + std::numeric_limits<JsonUInt>::digits10 };

// writer currently padding only for writeFloat(), reuse existing buffer for extra data
static constexpr auto Base10FloatDecimalPlaces =
    FloatParts::decimalPlacesForType<JsonFloat>();
static_assert(Base10FloatDecimalPlaces > 0, "");

struct Base10 {
  using buffer_type = char[1 + Base10UIntDigits + Base10FloatDecimalPlaces];

  explicit Base10(JsonUInt value) {
    auto* it = wend();

    *(it--) = '\0';
    while (value >= JsonUInt(10)) {
      *(it--) = char('0' + (value % JsonUInt(10)));
      value = JsonUInt(value / JsonUInt(10));
    }

    *it = char('0' + (value % JsonUInt(10)));
    _data = it;
  }

  const char* data() const {
    return _data;
  }

  const char* c_str() const {
    return data();
  }

  const char* begin() const {
   return data();
  }

  const char* end() const {
    return &_buffer[sizeof(_buffer) - 1];
  }

  size_t length() const {
    return static_cast<size_t>(end() - begin());
  }

 protected:
  char* wbegin() {
    return &_buffer[0];
  }

  char* wend() {
    return &_buffer[sizeof(_buffer) - 1];
  }

  buffer_type _buffer;
  char* _data;
};

struct PaddedBase10 : public Base10 {
  PaddedBase10(JsonUInt value, size_t padding) :
    Base10(value)
  {
    padding = Min(static_cast<size_t>(Base10FloatDecimalPlaces), padding);
    if (Base10::length() < padding) {
      const auto left = padding - Base10::length();
      const auto* left_begin = _data - left;
      while (_data != left_begin) {
        *(--_data) = '0';
      }
    }

    padding = Min(Base10::length(), padding);
    _data[padding] = '\0';
  }

 private:
  using Base10::Base10;
  using Base10::operator=;
  using Base10::begin;
  using Base10::end;
  using Base10::length;
};

}

class JsonWriterSinkBase {
 public:
  virtual size_t writeRaw(const char *) const = 0;
  virtual size_t writeRaw(char) const = 0;
 protected:
  ~JsonWriterSinkBase() = default;
};

// Writes the JSON tokens to a Print implementation
// This class currently expects:
// - size_t Print::print(const char *)
// - size_t Print::print(c)
template <typename Print>
class JsonWriterSink final : public JsonWriterSinkBase {
 private:
  Print& _sink;

 public:
  explicit JsonWriterSink(Print &sink) :
    _sink(sink)
  {}

  ~JsonWriterSink() = default;

  JsonWriterSink &operator=(const JsonWriterSink &) = delete;
  JsonWriterSink(const JsonWriterSink &) = delete;

  JsonWriterSink(JsonWriterSink &&) = delete;
  JsonWriterSink &operator=(JsonWriterSink &&) = delete;

  size_t writeRaw(const char *s) const override {
    if (s)
      return _sink.print(s);

    return 0;
  }

  size_t writeRaw(char c) const override {
    return _sink.print(c);
  }
};

// Writes the JSON tokens to a class providing sink interface
// This class is used by:
// - JsonArray::writeTo()
// - JsonObject::writeTo()
// - JsonVariant::writeTo()
class JsonWriter {
 private:
  JsonWriterSinkBase *_sink;
  size_t _bytesWritten{};

 public:
  JsonWriter() = delete;

  explicit JsonWriter(JsonWriterSinkBase *sink) :
    _sink(sink)
  {}

  ~JsonWriter() = default;

  JsonWriter(const JsonWriter &) = default;
  JsonWriter &operator=(const JsonWriter &) = delete;

  JsonWriter(JsonWriter &&) = default;
  JsonWriter &operator=(JsonWriter &&) = delete;

  // Returns the number of bytes sent to the Print implementation.
  // This is very handy for implementations of printTo() that must return the
  // number of bytes written.
  size_t bytesWritten() const {
    return _bytesWritten;
  }

  void writeRaw(const char *s) {
    _bytesWritten += _sink->writeRaw(s);
  }

  void writeRaw(char c) {
    _bytesWritten += _sink->writeRaw(c);
  }

  void beginArray() {
    writeRaw('[');
  }
  void endArray() {
    writeRaw(']');
  }

  void beginObject() {
    writeRaw('{');
  }
  void endObject() {
    writeRaw('}');
  }

  void writeColon() {
    writeRaw(':');
  }
  void writeComma() {
    writeRaw(',');
  }

  void writeBoolean(bool value) {
    writeRaw(value ? "true" : "false");
  }

  void writeNull() {
    writeRaw("null");
  }

  void writeString(const char *value) {
    writeRaw('\"');
    if (value) {
      char c;
      while ((c = Strings::Copy::Operator(value++)) != '\0')
        writeChar(c);
    }
    writeRaw('\"');
  }

  void writeChar(char c) {
    char specialChar = Encoding::escapeChar(c);
    if (specialChar) {
      writeRaw('\\');
      writeRaw(specialChar);
    } else {
      writeRaw(c);
    }
  }

  void writeNaN() {
    writeRaw("NaN");
  }

  void writeInfinity() {
    writeRaw("Infinity");
  }

  template <typename TFloat>
  void writeFloat(TFloat value) {
    if (isNaN(value)) {
      writeNaN();
      return;
    }

    if (SignBit(value)) {
      writeRaw('-');
      value = -value;
    }

    if (isInfinity(value)) {
      writeInfinity();
      return;
    }

    if (value == TFloat(0)) {
      writeRaw('0');
      return;
    }

    if (value == TFloat(1)) {
      writeRaw('1');
      return;
    }

    writeFloat(FloatParts::make(value));
  }

  void writeFloat(FloatParts parts) {
    writeInteger(parts.integral);
    if (parts.decimalPlaces > 0) {
      writeRaw('.');
      writeInteger(parts.decimal, static_cast<size_t>(parts.decimalPlaces));
    }

    const auto exponent = static_cast<unsigned>(
        parts.exponent < 0
            ? -parts.exponent
            : parts.exponent);

    if (parts.exponent < 0) {
      writeRaw('e');
      writeRaw('-');
      writeInteger(exponent);
    }

    if (parts.exponent > 0) {
      writeRaw('e');
      writeInteger(exponent);
    }
  }

  template <typename UInt>
  void writeInteger(UInt value) {
    const auto repr = JsonNumberWriter::Base10(value);
    writeRaw(repr.c_str());
  }

  void writeInteger(uint32_t value, size_t padding) {
    const auto repr = JsonNumberWriter::PaddedBase10(value, padding);
    writeRaw(repr.c_str());
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
