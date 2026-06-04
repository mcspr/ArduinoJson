// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Constant.hpp"

namespace ArduinoJson {
namespace Internals {

// A meta-function that returns true if types T and U are the same.
template <typename T, typename U>
struct IsSame : FalseType {
};

template <typename T>
struct IsSame<T, T> : TrueType {
};

}  // namespace Internals
}  // namespace ArduinoJson
