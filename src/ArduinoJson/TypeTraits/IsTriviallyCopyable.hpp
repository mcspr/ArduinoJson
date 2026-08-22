// MIT License

#pragma once

#include "Constant.hpp"

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

// gcc4.x missing full -std=c++11 support
template <typename T>
struct IsTriviallyCopyableImpl :
#if !defined(__clang__) && defined(__GNUC__) && __GNUC__ < 5
  BooleanConstant<__has_trivial_copy(T)>::type {
#else
  BooleanConstant<__is_trivially_copyable(T)>::type {
#endif
};

}

template <typename T>
struct IsTriviallyCopyable : TypeTraits::IsTriviallyCopyableImpl<T>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
