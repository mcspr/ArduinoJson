// MIT License

#pragma once

#include "Constant.hpp"

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

template <typename T>
struct IsPointer : FalseType {
};

template <typename T>
struct IsPointer<T*> : TrueType {
};

template <typename T>
struct IsPointer<T* const> : TrueType {
};

template <typename T>
struct IsPointer<T* volatile> : TrueType {
};

template <typename T>
struct IsPointer<T* const volatile> : TrueType {
};

template <>
struct IsPointer<std::nullptr_t> : TrueType {
};

}  // namespace Internals
}  // namespace ArduinoJson
