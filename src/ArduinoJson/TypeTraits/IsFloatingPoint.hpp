// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Constant.hpp"
#include "IsSame.hpp"

namespace ArduinoJson {
namespace Internals {

// A meta-function that returns true if T is a floating point type
template <typename T>
struct IsFloatingPoint : IntegralConstant<bool,
    IsSame<T, float>::value ||
    IsSame<T, double>::value> {
};

}  // namespace Internals
}  // namespace ArduinoJson
