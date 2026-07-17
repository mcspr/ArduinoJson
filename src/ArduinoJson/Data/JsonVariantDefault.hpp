// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

template <typename T>
struct JsonVariantDefault {
  static T get() {
    return T();
  }
};

template <typename T>
struct JsonVariantDefault<const T> : JsonVariantDefault<T> {};

template <typename T>
struct JsonVariantDefault<T&> : JsonVariantDefault<T> {};

template <typename T>
struct JsonVariantDefault<T&&> : JsonVariantDefault<T> {};

template <typename T>
struct JsonVariantDefault<T*> {
  static T* get() {
    return nullptr;
  }
};

template <typename TChar, size_t Size>
struct JsonVariantDefault<TChar[Size]> {
  static TChar* get() {
    return nullptr;
  }
};

template <typename TChar, size_t Size>
struct JsonVariantDefault<TChar(&)[Size]> {
  static TChar* get() {
    return nullptr;
  }
};

template <>
struct JsonVariantDefault<void> {
  static void get() {
  }
};

}  // namespace Internals
}  // namespace ArduinoJson
