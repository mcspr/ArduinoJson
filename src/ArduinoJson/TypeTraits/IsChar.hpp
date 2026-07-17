// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Constant.hpp"
#include "IsSame.hpp"

namespace ArduinoJson {
namespace Internals {

// A meta-function that returns true if T is a charater
template <typename T>
struct IsChar : IntegralConstant<bool,
    IsSame<T, char>::value ||
    IsSame<T, signed char>::value ||
    IsSame<T, unsigned char> ::value> {
};

template <typename T>
struct IsChar<const T> : IsChar<T> {
};

}  // namespace Internals
}  // namespace ArduinoJson
