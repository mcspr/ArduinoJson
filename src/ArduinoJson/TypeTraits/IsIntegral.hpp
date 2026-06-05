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
namespace TypeTraitsImpl {

// A meta-function that returns true if T is a type that is signed.
template <typename T>
struct IsSignedType :
      Or<IsSame<T, signed>,
         IsSame<T, signed char>,
         IsSame<T, signed short>,
         IsSame<T, signed int>,
         IsSame<T, signed long>,
         IsSame<T, signed long int>,
#if ARDUINOJSON_USE_LONG_LONG
        IsSame<T, signed long long>,
        IsSame<T, signed long long int>,
#endif
#if ARDUINOJSON_USE_INT64
        IsSame<T, unsigned __int64>,
#endif
        FalseType>::type {
};

// A meta-function that returns true if T is a type that is unsigned.
template <typename T>
struct IsUnsignedType :
      Or<IsSame<T, unsigned char>,
         IsSame<T, unsigned short>,
         IsSame<T, unsigned int>,
         IsSame<T, unsigned long>,
         IsSame<T, unsigned long int>,
#if ARDUINOJSON_USE_LONG_LONG
         IsSame<T, unsigned long long>,
         IsSame<T, unsigned long long int>,
#endif
#if ARDUINOJSON_USE_INT64
         IsSame<T, unsigned __int64>,
#endif
        FalseType>::type {
};

template <typename T>
struct IsUnsignedType<const T> : IsUnsignedType<T>::type {
};

}

// A meta-function that returns true if T is an integral type.
template <typename T>
struct IsIntegral :
    Or<TypeTraitsImpl::IsSignedType<T>,
       TypeTraitsImpl::IsUnsignedType<T>,
       IsSame<T, char>,
       IsSame<T, bool>>::type {
};

template <typename T>
struct IsIntegral<const T> : IsIntegral<T> {
};

}  // namespace Internals
}  // namespace ArduinoJson
