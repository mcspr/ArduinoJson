// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#ifdef _MSC_VER  // Visual Studio

#define ARDUINOJSON_FORCE_INLINE  // __forceinline causes C4714 when returning std::string
#define ARDUINOJSON_NO_INLINE __declspec(noinline)
#define ARDUINOJSON_DEPRECATED(msg) __declspec(deprecated(msg))

#elif defined(__GNUC__)  // GCC or Clang

#define ARDUINOJSON_FORCE_INLINE __attribute__((always_inline))
#define ARDUINOJSON_NO_INLINE __attribute__((noinline))
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
#define ARDUINOJSON_DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
#define ARDUINOJSON_DEPRECATED(msg) __attribute__((deprecated))
#endif

#else  // Other compilers

#define ARDUINOJSON_FORCE_INLINE
#define ARDUINOJSON_NO_INLINE
#define ARDUINOJSON_DEPRECATED(msg)

#endif
