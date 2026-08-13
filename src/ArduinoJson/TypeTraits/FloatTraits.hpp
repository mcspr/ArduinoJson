// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Polyfills/bit_cast.hpp"

#include "../TypeTraits/Constant.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsSignedIntegral.hpp"
#include "../TypeTraits/IsUnsignedIntegral.hpp"

#include <cstdint>
#include <cstddef>

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

  template <typename TExponent>
  static T make_float(T m, TExponent e) {
    if (e > 0) {
      for (uint8_t index = 0; e != 0; index++) {
        if (index >= binaryPowersOfTen) return nan();
        if (e & 1) m *= positiveBinaryPowerOfTen(index);
        e >>= 1;
      }
    } else {
      e = TExponent(-e);
      for (uint8_t index = 0; e != 0; index++) {
        if (index >= binaryPowersOfTen) return nan();
        if (e & 1) m *= negativeBinaryPowerOfTen(index);
        e >>= 1;
      }
    }
    return m;
  }

  static constexpr size_t binaryPowersOfTen = 9;

  static T positiveBinaryPowerOfTen(int index) {
    static T factors[binaryPowersOfTen] = {
        1e1,
        1e2,
        1e4,
        1e8,
        1e16,
        forge(0x4693B8B5, 0xB5056E17),  // 1e32
        forge(0x4D384F03, 0xE93FF9F5),  // 1e64
        forge(0x5A827748, 0xF9301D32),  // 1e128
        forge(0x75154FDD, 0x7F73BF3C)   // 1e256
    };
    return factors[index];
  }

  static T negativeBinaryPowerOfTen(int index) {
    static T factors[binaryPowersOfTen] = {
        forge(0x3FB99999, 0x9999999A),  // 1e-1
        forge(0x3F847AE1, 0x47AE147B),  // 1e-2
        forge(0x3F1A36E2, 0xEB1C432D),  // 1e-4
        forge(0x3E45798E, 0xE2308C3A),  // 1e-8
        forge(0x3C9CD2B2, 0x97D889BC),  // 1e-16
        forge(0x3949F623, 0xD5A8A733),  // 1e-32
        forge(0x32A50FFD, 0x44F4A73D),  // 1e-64
        forge(0x255BBA08, 0xCF8C979D),  // 1e-128
        forge(0x0AC80628, 0x64AC6F43)   // 1e-256
    };
    return factors[index];
  }

  static T negativeBinaryPowerOfTenPlusOne(int index) {
    static T factors[] = {
        1e0,
        forge(0x3FB99999, 0x9999999A),  // 1e-1
        forge(0x3F50624D, 0xD2F1A9FC),  // 1e-3
        forge(0x3E7AD7F2, 0x9ABCAF48),  // 1e-7
        forge(0x3CD203AF, 0x9EE75616),  // 1e-15
        forge(0x398039D6, 0x65896880),  // 1e-31
        forge(0x32DA53FC, 0x9631D10D),  // 1e-63
        forge(0x25915445, 0x81B7DEC2),  // 1e-127
        forge(0x0AFE07B2, 0x7DD78B14)   // 1e-255
    };
    return factors[index];
  }

  static T nan() {
    return forge(0x7ff80000, 0x00000000);
  }

  static T inf() {
    return forge(0x7ff00000, 0x00000000);
  }

  // constructs a double floating point values from its binary representation
  // we use this function to workaround platforms with single precision literals
  // (for example, when -fsingle-precision-constant is passed to GCC)
  static T forge(uint32_t msb, uint32_t lsb) {
    return bit_cast<T>((uint64_t(msb) << 32) | lsb);
  }

  template <typename TOut, typename EnableIf<
    TypeTraits::IsQuadWordSigned<TOut>::value, signed>::type* = nullptr>
  static T highest_for() {  // int64_t
    return forge(0x43DFFFFF, 0xFFFFFFFF);  //  9.2233720368547748e+18
  }

  template <typename TOut, typename EnableIf<
    TypeTraits::IsQuadWordUnsigned<TOut>::value, unsigned>::type* = nullptr>
  static T highest_for() {  // uint64_t
    return forge(0x43EFFFFF, 0xFFFFFFFF);  //  1.8446744073709549568e+19
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

  template <typename TExponent>
  static T make_float(T m, TExponent e) {
    if (e > 0) {
      for (uint8_t index = 0; e != 0; index++) {
        if (index >= binaryPowersOfTen) return nan();
        if (e & 1) m *= positiveBinaryPowerOfTen(index);
        e >>= 1;
      }
    } else {
      e = -e;
      for (uint8_t index = 0; e != 0; index++) {
        if (index >= binaryPowersOfTen) return nan();
        if (e & 1) m *= negativeBinaryPowerOfTen(index);
        e >>= 1;
      }
    }
    return m;
  }

  static constexpr size_t binaryPowersOfTen = 6;

  static T positiveBinaryPowerOfTen(int index) {
    static T factors[] = {1e1f, 1e2f, 1e4f, 1e8f, 1e16f, 1e32f};
    return factors[index];
  }

  static T negativeBinaryPowerOfTen(int index) {
    static T factors[] = {1e-1f, 1e-2f, 1e-4f, 1e-8f, 1e-16f, 1e-32f};
    return factors[index];
  }

  static T negativeBinaryPowerOfTenPlusOne(int index) {
    static T factors[] = {1e0f, 1e-1f, 1e-3f, 1e-7f, 1e-15f, 1e-31f};
    return factors[index];
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
    return forge(0x4EFFFFFF);  // 2.14748352E9
  }

  template <typename TOut, typename EnableIf<
    TypeTraits::IsDoubleWordUnsigned<TOut>::value, unsigned>::type* = nullptr>
  static T highest_for() {  // uint32_t
    return forge(0x4F7FFFFF);  // 4.29496704E9
  }

  template <typename TOut, typename EnableIf<
    TypeTraits::IsQuadWordSigned<TOut>::value, signed>::type* = nullptr>
  static T highest_for() {  // int64_t
    return forge(0x5EFFFFFF);  // 9.22337148709896192E18
  }

  template <typename TOut, typename EnableIf<
    TypeTraits::IsQuadWordUnsigned<TOut>::value, unsigned>::type* = nullptr>
  static T highest_for() {  // uint64_t
    return forge(0x5F7FFFFF);  // 1.844674297419792384E19
  }
};

}  // namespace Internals
}  // namespace ArduinoJson
