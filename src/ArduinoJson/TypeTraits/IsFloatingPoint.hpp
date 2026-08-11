// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Constant.hpp"

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

template <typename>
struct IsFloatingPointType : FalseType {
};

template <>
struct IsFloatingPointType<float> : TrueType {
};

template <>
struct IsFloatingPointType<double> : TrueType {
};

template <typename T>
struct IsFloatingPointImpl : IsFloatingPointType<T>::type {
};

template <typename T>
struct IsFloatingPointImpl<T const> : IsFloatingPointType<T>::type {
};

template <typename T>
struct IsFloatingPointImpl<T volatile> : IsFloatingPointType<T>::type {
};

template <typename T>
struct IsFloatingPointImpl<T const volatile> : IsFloatingPointType<T>::type {
};

}

// A meta-function that returns true if T is a floating point type
template <typename T>
struct IsFloatingPoint : TypeTraits::IsFloatingPointImpl<T>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
