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

template <typename>
struct IsPlainCharType : FalseType {
};

template <>
struct IsPlainCharType<char> : TrueType {
};

template <typename T>
struct IsPlainCharImpl : IsPlainCharType<T>::type {
};

template <typename T>
struct IsPlainCharImpl<T const> : IsPlainCharType<T>::type {
};

template <typename T>
struct IsPlainCharImpl<T volatile> : IsPlainCharType<T>::type {
};

template <typename T>
struct IsPlainCharImpl<T const volatile> : IsPlainCharType<T>::type {
};

}

// A meta-function that returns true if T is a charater
template <typename T>
struct IsChar : TypeTraits::IsCharImpl<T>::type {
};

// A meta-function that returns true if T is a plain char (or whatever signed or unsigned type would match it)
template <typename T>
struct IsPlainChar : TypeTraits::IsPlainCharImpl<T>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
