// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "IsSame.hpp"
#include "Or.hpp"

namespace ArduinoJson {
namespace Internals {

// A meta-function that returns true if T is a floating point type
template <typename T>
struct IsFloatingPoint : Or<
    IsSame<T, float>,
    IsSame<T, double>>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
