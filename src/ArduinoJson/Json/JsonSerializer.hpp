// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Json/TextFormatter.hpp>
#include <ArduinoJson/Serialization/measure.hpp>
#include <ArduinoJson/Serialization/serialize.hpp>
#include <ArduinoJson/Variant/VariantDataVisitor.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename TWriter>
class JsonSerializer : public VariantDataVisitor<size_t> {
 public:
  static const bool producesText = true;

  JsonSerializer(TWriter writer, ResourceManager* resources)
      : formatter_(writer), resources_(resources) {}

  size_t visitArray(const VariantImpl& array) {
    write('[');

    bool first = true;

    for (auto it = array.createIterator(); !it.done(); it.move()) {
      if (!first)
        write(',');

      it->accept(*this);
      first = false;
    }

    write(']');
    return bytesWritten();
  }

  size_t visitObject(const VariantImpl& object) {
    write('{');

    bool first = true;
    bool isValue = false;

    for (auto it = object.createIterator(); !it.done(); it.move()) {
      if (isValue)
        write(':');
      else if (!first)
        write(',');

      it->accept(*this);

      first = false;
      isValue = !isValue;
    }

    write('}');
    return bytesWritten();
  }

  template <typename T>
  enable_if_t<is_floating_point<T>::value, size_t> visit(T value) {
    formatter_.writeFloat(value);
    return bytesWritten();
  }

  size_t visit(const char* value) {
    formatter_.writeString(value);
    return bytesWritten();
  }

  size_t visit(JsonString value) {
    formatter_.writeString(value.c_str(), value.size());
    return bytesWritten();
  }

  size_t visit(RawString value) {
    formatter_.writeRaw(value.data(), value.size());
    return bytesWritten();
  }

  size_t visit(JsonInteger value) {
    formatter_.writeInteger(value);
    return bytesWritten();
  }

  size_t visit(JsonUInt value) {
    formatter_.writeInteger(value);
    return bytesWritten();
  }

  size_t visit(bool value) {
    formatter_.writeBoolean(value);
    return bytesWritten();
  }

  size_t visit(nullptr_t) {
    formatter_.writeRaw("null");
    return bytesWritten();
  }

 protected:
  size_t bytesWritten() const {
    return formatter_.bytesWritten();
  }

  void write(char c) {
    formatter_.writeRaw(c);
  }

  void write(const char* s) {
    formatter_.writeRaw(s);
  }

 private:
  TextFormatter<TWriter> formatter_;

 protected:
  ResourceManager* resources_;
};

ARDUINOJSON_END_PRIVATE_NAMESPACE

ARDUINOJSON_BEGIN_PUBLIC_NAMESPACE

// Produces a minified JSON document.
// https://arduinojson.org/v7/api/json/serializejson/
template <
    typename TDestination,
    detail::enable_if_t<!detail::is_pointer<TDestination>::value, int> = 0>
size_t serializeJson(JsonVariantConst source, TDestination& destination) {
  using namespace detail;
  return serialize<JsonSerializer>(source, destination);
}

// Produces a minified JSON document.
// https://arduinojson.org/v7/api/json/serializejson/
inline size_t serializeJson(JsonVariantConst source, void* buffer,
                            size_t bufferSize) {
  using namespace detail;
  return serialize<JsonSerializer>(source, buffer, bufferSize);
}

// Computes the length of the document that serializeJson() produces.
// https://arduinojson.org/v7/api/json/measurejson/
inline size_t measureJson(JsonVariantConst source) {
  using namespace detail;
  return measure<JsonSerializer>(source);
}

#if ARDUINOJSON_ENABLE_STD_STREAM
template <typename T,
          detail::enable_if_t<
              detail::is_convertible<T, JsonVariantConst>::value, int> = 0>
inline std::ostream& operator<<(std::ostream& os, const T& source) {
  serializeJson(source, os);
  return os;
}
#endif

ARDUINOJSON_END_PUBLIC_NAMESPACE
