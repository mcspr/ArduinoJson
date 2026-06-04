// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#include "Constant.hpp"
#include "IsSame.hpp"
#include "Or.hpp"

namespace ArduinoJson {
namespace Internals {

// A meta-function that returns true if T is an integral type.
template <typename T>
struct IsSignedIntegral : Or<
    IsSame<T, signed char>,
    IsSame<T, signed short>,
    IsSame<T, signed int>,
    IsSame<T, signed long>,
#if ARDUINOJSON_USE_LONG_LONG
    IsSame<T, signed long long>,
#endif
#if ARDUINOJSON_USE_INT64
    IsSame<T, signed __int64>,
#endif
    FalseType>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
