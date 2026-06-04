// MIT License

#pragma once

#include "Constant.hpp"
#include "EnableIf.hpp"

namespace ArduinoJson {
namespace Internals {
namespace AndImpl {

// Helper template to replace various EnableIf<A && B && C> etc. involving true or false types
// Both to shorten the checks, as well as short-circuit otherwise faulty expressions behind EnableIf
// (e.g. Is{Un,Si}gned<const char*> implemented by T(-1) < T(0) is not constexpr, IsIntegral<...> should be checked first)

template<typename T, typename...>
using FirstType = T;

template <typename... Boolean>
FirstType<TrueType, typename Internals::EnableIf<bool(Boolean::value)>::type...>
AndFunction(int);

template <typename... Boolean>
FalseType AndFunction(...);

template<typename... Boolean>
struct And : decltype(AndFunction<Boolean...>(0)) {
};

}

// aka -std=c++17 std::conjunction
using AndImpl::And;

}
}
