// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Constant.hpp"

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

// A meta-function that return the type T without the const modifier
template <typename T>
struct IsArray : FalseType {
};

template <typename T>
struct IsArray<T[]> : TrueType {
};

template <typename T, size_t N>
struct IsArray<T[N]> : TrueType {
};
}  // namespace Internals
}  // namespace ArduinoJson
