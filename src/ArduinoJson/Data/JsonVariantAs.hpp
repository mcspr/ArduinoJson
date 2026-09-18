// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

// A metafunction that returns the type of the value returned by
// JsonVariant::as<T>()
template <typename T>
struct JsonVariantAs {
  typedef T type;
};

template <>
struct JsonVariantAs<const char*> {
  typedef const char* type;
};

template <>
struct JsonVariantAs<char*> {
  typedef const char* type;
};

template <>
struct JsonVariantAs<unsigned char*> {
  typedef const char* type;
};

template <>
struct JsonVariantAs<const unsigned char*> {
  typedef const char* type;
};

template <typename TChar, size_t Size>
struct JsonVariantAs<TChar[Size]> {
  typedef const char* type;
};

template <typename TChar, size_t Size>
struct JsonVariantAs<TChar(&)[Size]> {
  typedef const char* type;
};

// A metafunction that *prefers* to return the const reference type of the value returned by
// JsonVariant::as<T>()
template <typename T>
struct JsonVariantAsConst : JsonVariantAs<T> {
};

template <typename T>
struct JsonVariantAsConst<T&> : JsonVariantAs<const T&> {
};

template <typename T>
struct JsonVariantAsConst<const T&> : JsonVariantAs<const T&> {
};

}  // namespace Internals
}  // namespace ArduinoJson
