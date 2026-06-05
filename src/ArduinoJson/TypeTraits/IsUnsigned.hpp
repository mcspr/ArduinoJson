// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "IsSigned.hpp"
#include "Not.hpp"

namespace ArduinoJson {
namespace Internals {
namespace TypeTraitsImpl {

template <typename T>
struct IsUnsigned : Not<IsSigned<T>>::type {
};

}

// A meta-function that returns true if T is an unsigned type.
template <typename T>
struct IsUnsigned : TypeTraitsImpl::IsUnsigned<T>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
