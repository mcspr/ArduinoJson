// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Constant.hpp"
#include "IsSame.hpp"
#include "IsSignedIntegral.hpp"
#include "IsUnsignedIntegral.hpp"

namespace ArduinoJson {
namespace Internals {

// A meta-function that returns true if T is an integral type.
// CAUTION: differs from std::is_integral as it doesn't include bool
template <typename T>
struct IsIntegral : IntegralConstant<bool,
    IsSignedIntegral<T>::value ||
    IsUnsignedIntegral<T>::value ||
    IsSame<T, char>::value> {
};

template <typename T>
struct IsIntegral<const T> : IsIntegral<T> {
};

}  // namespace Internals
}  // namespace ArduinoJson
