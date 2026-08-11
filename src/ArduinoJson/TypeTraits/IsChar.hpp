// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Constant.hpp"

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

template <typename>
struct IsCharType : FalseType {
};

template <>
struct IsCharType<char> : TrueType {
};

template <>
struct IsCharType<signed char> : TrueType {
};

template <>
struct IsCharType<unsigned char> : TrueType {
};

template <typename T>
struct IsCharImpl : IsCharType<T>::type {
};

template <typename T>
struct IsCharImpl<T const> : IsCharType<T>::type {
};

template <typename T>
struct IsCharImpl<T volatile> : IsCharType<T>::type {
};

template <typename T>
struct IsCharImpl<T const volatile> : IsCharType<T>::type {
};

}

// A meta-function that returns true if T is a charater
template <typename T>
struct IsChar : TypeTraits::IsCharImpl<T>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
