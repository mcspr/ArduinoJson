// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#include "Constant.hpp"
#include "Or.hpp"

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

template <typename>
struct IsSignedTypeImpl : FalseType {
};

template <>
struct IsSignedTypeImpl<signed char> : TrueType {
};

template <>
struct IsSignedTypeImpl<signed short> : TrueType {
};

template <>
struct IsSignedTypeImpl<signed int> : TrueType {
};

template <>
struct IsSignedTypeImpl<signed long> : TrueType {
};

#if ARDUINOJSON_USE_LONG_LONG
template <>
struct IsSignedTypeImpl<signed long long> : TrueType {
};
#endif

#if ARDUINOJSON_USE_INT64
template <>
struct IsSignedTypeImpl<signed __int64> : TrueType {
};
#endif

// A meta-function that returns true if T is a type that is signed.
template <typename T>
struct IsSignedType : IsSignedTypeImpl<T>::type {
};

template <typename T>
struct IsSignedType<T const> : IsSignedTypeImpl<T>::type {
};

template <typename T>
struct IsSignedType<T volatile> : IsSignedTypeImpl<T>::type {
};

template <typename T>
struct IsSignedType<T const volatile> : IsSignedTypeImpl<T>::type {
};

template <typename>
struct IsUnsignedTypeImpl : FalseType {
};

template <>
struct IsUnsignedTypeImpl<unsigned char> : TrueType {
};

template <>
struct IsUnsignedTypeImpl<unsigned short> : TrueType {
};

template <>
struct IsUnsignedTypeImpl<unsigned int> : TrueType {
};

template <>
struct IsUnsignedTypeImpl<unsigned long> : TrueType {
};

#if ARDUINOJSON_USE_LONG_LONG
template <>
struct IsUnsignedTypeImpl<unsigned long long> : TrueType {
};
#endif

#if ARDUINOJSON_USE_INT64
template <>
struct IsUnsignedTypeImpl<unsigned __int64> : TrueType {
};
#endif

// A meta-function that returns true if T is a type that is unsigned.
template <typename T>
struct IsUnsignedType : IsUnsignedTypeImpl<T>::type {
};

template <typename T>
struct IsUnsignedType<T const> : IsUnsignedType<T>::type {
};

template <typename T>
struct IsUnsignedType<T volatile> : IsUnsignedType<T>::type {
};

template <typename T>
struct IsUnsignedType<T const volatile> : IsUnsignedType<T>::type {
};

// A meta-function that matches integral types that are neither explicitly signed or unsigned
template <typename>
struct IsIntegralType : FalseType {
};

template <>
struct IsIntegralType<char> : TrueType {
};

template <>
struct IsIntegralType<bool> : TrueType {
};

// A meta-function that combines all of the type checkers above
template <typename T>
struct IsIntegralTypeImpl :
    Or<IsSignedType<T>,
       IsUnsignedType<T>,
       IsIntegralType<T>>::type {

};

template <typename T>
struct IsIntegralImpl : IsIntegralTypeImpl<T>::type {
};

template <typename T>
struct IsIntegralImpl<T const > : IsIntegralTypeImpl<T>::type {
};

template <typename T>
struct IsIntegralImpl<T volatile> : IsIntegralTypeImpl<T>::type {
};

template <typename T>
struct IsIntegralImpl<T const volatile> : IsIntegralTypeImpl<T>::type {
};

}

// A meta-function that returns true if T is an integral type.
template <typename T>
struct IsIntegral : TypeTraits::IsIntegralImpl<T>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
