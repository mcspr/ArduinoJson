// MIT License

#pragma once

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

template <typename T>
struct IsPointer {
  static const bool value = false;
};

template <typename T>
struct IsPointer<T*> {
  static const bool value = true;
};

template <typename T>
struct IsPointer<T* const> {
  static const bool value = true;
};

template <>
struct IsPointer<std::nullptr_t> {
  static const bool value = true;
};
}  // namespace Internals
}  // namespace ArduinoJson
