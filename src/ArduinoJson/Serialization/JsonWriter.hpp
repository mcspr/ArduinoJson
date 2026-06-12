// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Strings/Strings.hpp"
#include "../Data/Encoding.hpp"
#include "../Data/JsonInteger.hpp"
#include "../Serialization/FloatParts.hpp"
#include "../Polyfills/math.hpp"

#include <cstdint>
#include <limits>

namespace ArduinoJson {
namespace Internals {
namespace JsonIntegerWriter {

struct Base10 {
  static constexpr auto actual_digits = std::numeric_limits<JsonUInt>::digits10;
  static constexpr auto buffer_digits = actual_digits + 10;
  using buffer_type = char[Base10::buffer_digits];

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

 private:
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
    auto* out = wbegin();
    if (Base10::length() < padding) {
      size_t left = padding - Base10::length();
      while (left--)
        *(out++) = '0';
    }

    padding = Min(padding, Base10::length());
    const auto* end = Base10::data() + padding;
    for (auto it = Base10::data(); it != end; ++it) {
      *(out++) = *it;
    }

    *out = '\0';
  }

  const char* data() const {
    return &_buffer[0];
  }

  const char* c_str() const {
    return data();
  }

 private:
  using Base10::Base10;
  using Base10::operator=;
  using Base10::begin;
  using Base10::end;
  using Base10::length;

  char* wbegin() {
    return &_buffer[0];
  }

  buffer_type _buffer;
};

}

// Writes the JSON tokens to a Print implementation
// This class is used by:
// - JsonArray::writeTo()
// - JsonObject::writeTo()
// - JsonVariant::writeTo()
// Its derived by PrettyJsonWriter that overrides some members to add
// indentation.
template <typename Print>
class JsonWriter {
 public:
  explicit JsonWriter(Print &sink) :
    _sink(sink)
  {}

  JsonWriter &operator=(const JsonWriter &) = delete;
  JsonWriter &operator=(JsonWriter &&) = delete;

  // Returns the number of bytes sent to the Print implementation.
  // This is very handy for implementations of printTo() that must return the
  // number of bytes written.
  size_t bytesWritten() const {
    return _length;
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
    const auto repr = JsonIntegerWriter::Base10(value);
    writeRaw(repr.c_str());
  }

  void writeInteger(uint32_t value, size_t padding) {
    const auto repr = JsonIntegerWriter::PaddedBase10(value, padding);
    writeRaw(repr.c_str());
  }

  void writeRaw(const char *s) {
    if (s)
      _length += _sink.print(s);
  }
  void writeRaw(char c) {
    _length += _sink.print(c);
  }

 protected:
  Print &_sink;
  size_t _length{};
};
}  // namespace Internals
}  // namespace ArduinoJson
