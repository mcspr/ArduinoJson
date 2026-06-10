// MIT License

#pragma once

#include "Constant.hpp"
#include "EnableIf.hpp"
#include "FirstType.hpp"

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

// Helper template to replace various EnableIf<A || B || C> etc. involving true or false types
// Both to shorten the checks, as well as short-circuit otherwise faulty expressions behind EnableIf
// (e.g. Is{Un,Si}gned<const char*> implemented by T(-1) < T(0) is not constexpr, IsIntegral<...> should be checked first)

template <typename... Boolean>
FirstType<FalseType, typename Internals::EnableIf<!bool(Boolean::value)>::type...>
OrImpl(int);

template <typename... Boolean>
TrueType OrImpl(...);

}

// aka -std=c++17 std::disjunction
template <typename... Boolean>
struct Or : decltype(TypeTraits::OrImpl<Boolean...>(0)) {
};

}
}
