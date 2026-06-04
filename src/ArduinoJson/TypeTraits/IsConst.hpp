// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Constant.hpp"

namespace ArduinoJson {
namespace Internals {

// A meta-function that checks for the const qualifier of T
template <typename T>
struct IsConst : FalseType {
};

template <typename T>
struct IsConst<const T> : TrueType {
};

}  // namespace Internals
}  // namespace ArduinoJson
