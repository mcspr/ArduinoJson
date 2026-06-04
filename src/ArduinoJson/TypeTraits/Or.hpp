// MIT License

#pragma once

#include "Constant.hpp"
#include "EnableIf.hpp"

namespace ArduinoJson {
namespace Internals {
namespace OrImpl {

// Helper template to replace various EnableIf<A || B || C> etc. involving true or false types
// Both to shorten the checks, as well as short-circuit otherwise faulty expressions behind EnableIf
// (e.g. Is{Un,Si}gned<const char*> implemented by T(-1) < T(0) is not constexpr, IsIntegral<...> should be checked first)

template<typename T, typename...>
using FirstType = T;

template <typename... Boolean>
FirstType<FalseType, typename Internals::EnableIf<!bool(Boolean::value)>::type...>
OrFunction(int);

template <typename... Boolean>
TrueType OrFunction(...);

template <typename... Boolean>
struct Or : decltype(OrFunction<Boolean...>(0)) {
};

}

// aka -std=c++17 std::disjunction
using OrImpl::Or;

}
}
