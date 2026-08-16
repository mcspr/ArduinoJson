// MIT License

#pragma once

#include "Constant.hpp"
#include "And.hpp"

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

// gcc4.x missing full -std=c++11 support
template <typename T>
struct IsTrivialImpl : And<
#if !defined(__clang__) && defined(__GNUC__) && __GNUC__ < 5
  BooleanConstant<__has_trivial_copy(T)>,
  BooleanConstant<__has_trivial_destructor(T)>>::type {
#else
  BooleanConstant<__is_trivially_copyable(T)>,
  BooleanConstant<__is_trivially_destructible(T)>>::type {
#endif
};

}

template <typename T>
struct IsTrivial : TypeTraits::IsTrivialImpl<T>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
