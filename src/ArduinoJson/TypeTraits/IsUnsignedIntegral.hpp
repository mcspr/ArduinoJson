// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "IsIntegral.hpp"
#include "IsUnsigned.hpp"

#include "And.hpp"

namespace ArduinoJson {
namespace Internals {

// A meta-function that returns true if T is an integral type.
template <typename T>
struct IsUnsignedIntegral : And<IsIntegral<T>, IsUnsigned<T>>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
