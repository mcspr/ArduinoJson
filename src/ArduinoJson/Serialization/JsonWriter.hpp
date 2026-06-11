// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Strings/Strings.hpp"
#include "../Data/Encoding.hpp"
#include "../Data/JsonInteger.hpp"
#include "../Polyfills/attributes.hpp"
#include "../Serialization/FloatParts.hpp"

#include <cstdint>
#include <limits>

namespace ArduinoJson {
namespace Internals {
namespace JsonIntegerWriter {

struct Base10 {
  static constexpr auto digits = std::numeric_limits<JsonUInt>::digits10;

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

  char _buffer[digits + 3];
  char* _data;
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

  template <typename TFloat>
  void writeFloat(TFloat value) {
    if (isNaN(value)) {
      writeRaw("NaN");
      return;
    }

    if (value < 0.0) {
      writeRaw('-');
      value = -value;
    }

    if (isInfinity(value)) {
      writeRaw("Infinity");
      return;
    }

    FloatParts<TFloat> parts(value);

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
    const auto repr = JsonIntegerWriter::Base10(value);
    if (repr.length() < padding) {
      size_t left = padding - repr.length();
      while (left--)
        writeRaw('0');
    }

    padding = Min(padding, repr.length());
    const auto* end = repr.data() + padding;
    for (auto it = repr.data(); it != end; ++it) {
      writeRaw(*it);
    }
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
