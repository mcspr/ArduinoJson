// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Configuration.hpp"

#include "../Polyfills/bit_cast.hpp"

#include "../TypeTraits/Constant.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsSignedIntegral.hpp"
#include "../TypeTraits/IsUnsignedIntegral.hpp"

#include <cstdint>
#include <cstddef>

// rodata should also be explicitly placed into flash section, allowing to delay loading it into RAM

#if ARDUINOJSON_ENABLE_PROGMEM
#include <Arduino.h>
#define ARDUINOJSON_FLOAT_TRAITS_PROGMEM_ATTR PROGMEM
#else
#define ARDUINOJSON_FLOAT_TRAITS_PROGMEM_ATTR
#endif

// nb. for current application, floating point data is always addressable through the normal memory access funcs
// in case it is not, make sure that forge(...) below loads it through the necessary means first

#define ARDUINOJSON_FLOAT_TRAITS_FACTORS(TYPE)\
static constexpr TYPE factors[binaryPowersOfTen] ARDUINOJSON_FLOAT_TRAITS_PROGMEM_ATTR

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

// floating point conversion has to know output type size limits
template <typename TReference, typename TOther>
struct IsSameSize : BooleanConstant<sizeof(TReference) == sizeof(TOther)>::type {
};

// int32_t, uint32_t
template <typename T>
struct IsDoubleWordSigned :
    And<IsSignedIntegral<T>, IsSameSize<int32_t, T>>::type {
};

template <typename T>
struct IsDoubleWordUnsigned :
    And<IsUnsignedIntegral<T>, IsSameSize<int32_t, T>>::type {
};

// int64_t, uint64_t
template <typename T>
struct IsQuadWordSigned :
    And<IsSignedIntegral<T>, IsSameSize<int64_t, T>>::type {
};

template <typename T>
struct IsQuadWordUnsigned :
    And<IsUnsignedIntegral<T>, IsSameSize<int64_t, T>>::type {
};

}

template <typename T, size_t = sizeof(T)>
struct FloatTraits {
};

template <typename T>
struct FloatTraits<T, 8> {
  static constexpr short bits = 64;
  static constexpr short mantissa_bits = 52;

  using mantissa_type = int64_t;
  static constexpr mantissa_type mantissa_max =
      (static_cast<mantissa_type>(1) << mantissa_bits) - 1;

  using exponent_type = int16_t;
  static constexpr exponent_type exponent_min = -323;
  static constexpr exponent_type exponent_max = 308;

  static constexpr size_t binaryPowersOfTen = 9;

  static T positiveBinaryPowerOfTen(int index) {
    ARDUINOJSON_FLOAT_TRAITS_FACTORS(uint64_t) = {
      0x4024000000000000,  // 1e1
      0x4059000000000000,  // 1e2
      0x40c3880000000000,  // 1e4
      0x4197d78400000000,  // 1e8
      0x4341c37937e08000,  // 1e16
      0x4693b8b5b5056e17,  // 1e32
      0x4d384f03e93ff9f5,  // 1e64
      0x5a827748f9301d32,  // 1e128
      0x75154fdd7f73bf3c,  // 1e256
    };

    return forge(factors[index]);
  }

  static T negativeBinaryPowerOfTen(int index) {
    ARDUINOJSON_FLOAT_TRAITS_FACTORS(uint64_t) = {
      0x3fb999999999999a,  // 1e-1
      0x3f847ae147ae147b,  // 1e-2
      0x3f1a36e2eb1c432d,  // 1e-4
      0x3e45798ee2308c3a,  // 1e-8
      0x3c9cd2b297d889bc,  // 1e-16
      0x3949f623d5a8a733,  // 1e-32
      0x32a50ffd44f4a73d,  // 1e-64
      0x255bba08cf8c979d,  // 1e-128
      0x0ac8062864ac6f43   // 1e-256
    };

    return forge(factors[index]);
  }

  static T nan() {
    return forge(0x7ff8000000000000);
  }

  static T inf() {
    return forge(0x7ff0000000000000);
  }

  static T forge(uint64_t carrier) {
    return bit_cast<T>(carrier);
  }

  static T forge(uint32_t msb, uint32_t lsb) {
    return forge((static_cast<uint64_t>(msb) << (bits / 2)) | static_cast<uint64_t>(lsb));
  }

  template <typename TOut, typename EnableIf<
    TypeTraits::IsQuadWordSigned<TOut>::value, signed>::type* = nullptr>
  static T highest_for() {  // int64_t
    return forge(0x43dfffffffffffff);  //  9.2233720368547748e+18
  }

  template <typename TOut, typename EnableIf<
    TypeTraits::IsQuadWordUnsigned<TOut>::value, unsigned>::type* = nullptr>
  static T highest_for() {  // uint64_t
    return forge(0x43efffffffffffff);  //  1.8446744073709549568e+19
  }
};

template <typename T>
struct FloatTraits<T, 4> {
  static constexpr short bits = 32;
  static constexpr short mantissa_bits = 23;

  using mantissa_type = int32_t;
  static constexpr mantissa_type mantissa_max =
      (static_cast<mantissa_type>(1) << mantissa_bits) - 1;

  using exponent_type = int8_t;
  static constexpr exponent_type exponent_min = -45;
  static constexpr exponent_type exponent_max = 38;

  static constexpr size_t binaryPowersOfTen = 6;

  static T positiveBinaryPowerOfTen(int index) {
    ARDUINOJSON_FLOAT_TRAITS_FACTORS(uint32_t) = {
      0x41200000,  // 1e1f
      0x42c80000,  // 1e2f
      0x461c4000,  // 1e4f
      0x4cbebc20,  // 1e8f
      0x5a0e1bca,  // 1e16f
      0x749dc5ae,  // 1e32f
    };

    return forge(factors[index]);
  }

  static T negativeBinaryPowerOfTen(int index) {
    ARDUINOJSON_FLOAT_TRAITS_FACTORS(uint32_t) = {
      0x3dcccccd,  // 1e-1f
      0x3c23d70a,  // 1e-2f
      0x38d1b717,  // 1e-4f
      0x322bcc77,  // 1e-8f
      0x24e69595,  // 1e-16f
      0x0a4fb11f,  // 1e-32f
    };

    return forge(factors[index]);
  }

  static T forge(uint32_t carrier) {
    return bit_cast<T>(carrier);
  }

  static T nan() {
    return forge(0x7fc00000);
  }

  static T inf() {
    return forge(0x7f800000);
  }

  template <typename TOut, typename EnableIf<
    TypeTraits::IsDoubleWordSigned<TOut>::value, signed>::type* = nullptr>
  static T highest_for() {
    return forge(0x4effffff);  // 2.14748352E9
  }

  template <typename TOut, typename EnableIf<
    TypeTraits::IsDoubleWordUnsigned<TOut>::value, unsigned>::type* = nullptr>
  static T highest_for() {  // uint32_t
    return forge(0x4f7fffff);  // 4.29496704E9
  }

  template <typename TOut, typename EnableIf<
    TypeTraits::IsQuadWordSigned<TOut>::value, signed>::type* = nullptr>
  static T highest_for() {  // int64_t
    return forge(0x5effffff);  // 9.22337148709896192E18
  }

  template <typename TOut, typename EnableIf<
    TypeTraits::IsQuadWordUnsigned<TOut>::value, unsigned>::type* = nullptr>
  static T highest_for() {  // uint64_t
    return forge(0x5f7fffff);  // 1.844674297419792384E19
  }
};

#undef ARDUINOJSON_FLOAT_TRAITS_FACTORS
#undef ARDUINOJSON_FLOAT_TRAITS_PROGMEM_ATTR

}  // namespace Internals
}  // namespace ArduinoJson
