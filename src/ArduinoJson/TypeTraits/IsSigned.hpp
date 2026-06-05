// MIT License

#pragma once

#include "Constant.hpp"

namespace ArduinoJson {
namespace Internals {
namespace TypeTraitsImpl {

// nb. 7.x encodes both signed and unsigned checks as is_same<...signed types...>
// -funsigned-char exists, so doing the next best thing and just comparing some T values

template <typename T>
struct IsSigned : BooleanConstant<bool(T(-1) < T(0))>::type {
};

}

// A meta-function that returns true if T is a signed type
template <typename T>
struct IsSigned : TypeTraitsImpl::IsSigned<T>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson

