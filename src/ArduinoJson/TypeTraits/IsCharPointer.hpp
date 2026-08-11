// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "IsChar.hpp"

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

template <typename T>
struct IsCharPointerType : FalseType {
};

template <typename T>
struct IsCharPointerType<T*> : IsChar<T>::type {
};

template <typename>
struct IsPlainCharPointerType : FalseType {
};

template <typename T>
struct IsPlainCharPointerType<T*> : IsPlainChar<T>::type {
};

}

// A meta-function that returns true if T is a pointer that satisfies IsChar trait check
template <typename T>
struct IsCharPointer : TypeTraits::IsCharPointerType<T>::type {
};

// A meta-function that returns true if T is a pointer to a generic char type
template <typename T>
struct IsPlainCharPointer : TypeTraits::IsPlainCharPointerType<T>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
