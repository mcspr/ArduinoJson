// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

// Small or big machine?
#ifndef ARDUINOJSON_EMBEDDED_MODE
#if defined(ARDUINO) || defined(__IAR_SYSTEMS_ICC__) || defined(__XC) || \
    defined(__ARMCC_VERSION)
#define ARDUINOJSON_EMBEDDED_MODE 1
#else
#define ARDUINOJSON_EMBEDDED_MODE 0
#endif
#endif

#if ARDUINOJSON_EMBEDDED_MODE

// Store floats by default to reduce the memory usage (issue #134)
#ifndef ARDUINOJSON_USE_DOUBLE
#define ARDUINOJSON_USE_DOUBLE 0
#endif

// Store longs by default, because they usually match the size of a float.
#ifndef ARDUINOJSON_USE_LONG_LONG
#define ARDUINOJSON_USE_LONG_LONG 0
#endif
#ifndef ARDUINOJSON_USE_INT64
#define ARDUINOJSON_USE_INT64 0
#endif

// Embedded systems usually don't have std::string
#ifndef ARDUINOJSON_ENABLE_STD_STRING
#define ARDUINOJSON_ENABLE_STD_STRING 0
#endif

// Embedded systems usually don't have std::stream
#ifndef ARDUINOJSON_ENABLE_STD_STREAM
#define ARDUINOJSON_ENABLE_STD_STREAM 0
#endif

// Limit nesting as the stack is likely to be small
#ifndef ARDUINOJSON_DEFAULT_NESTING_LIMIT
#define ARDUINOJSON_DEFAULT_NESTING_LIMIT 10
#endif

#else  // ARDUINOJSON_EMBEDDED_MODE

// On a computer we have plenty of memory so we can use doubles
#ifndef ARDUINOJSON_USE_DOUBLE
#define ARDUINOJSON_USE_DOUBLE 1
#endif

// Use long long when available
#ifndef ARDUINOJSON_USE_LONG_LONG
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define ARDUINOJSON_USE_LONG_LONG 1
#else
#define ARDUINOJSON_USE_LONG_LONG 0
#endif
#endif

// Use _int64 on old versions of Visual Studio
#ifndef ARDUINOJSON_USE_INT64
#if defined(_MSC_VER) && _MSC_VER <= 1700
#define ARDUINOJSON_USE_INT64 1
#else
#define ARDUINOJSON_USE_INT64 0
#endif
#endif

// On a computer, we can use std::string
#ifndef ARDUINOJSON_ENABLE_STD_STRING
#define ARDUINOJSON_ENABLE_STD_STRING 1
#endif

// On a computer, we can assume std::stream
#ifndef ARDUINOJSON_ENABLE_STD_STREAM
#define ARDUINOJSON_ENABLE_STD_STREAM 1
#endif

// On a computer, the stack is large so we can increase nesting limit
#ifndef ARDUINOJSON_DEFAULT_NESTING_LIMIT
#define ARDUINOJSON_DEFAULT_NESTING_LIMIT 50
#endif

#endif  // ARDUINOJSON_EMBEDDED_MODE

#if ARDUINOJSON_DEFAULT_NESTING_LIMIT > 255
#error ARDUINOJSON_DEFAULT_NESTING_LIMIT larger than the type maximum
#endif

#ifdef ARDUINO

// Enable support for Arduino String
#ifndef ARDUINOJSON_ENABLE_ARDUINO_STRING
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
#endif

// Enable support for Arduino Stream
#ifndef ARDUINOJSON_ENABLE_ARDUINO_STREAM
#define ARDUINOJSON_ENABLE_ARDUINO_STREAM 1
#endif

#else  // ARDUINO

// Disable support for Arduino String
#ifndef ARDUINOJSON_ENABLE_ARDUINO_STRING
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 0
#endif

// Disable support for Arduino Stream
#ifndef ARDUINOJSON_ENABLE_ARDUINO_STREAM
#define ARDUINOJSON_ENABLE_ARDUINO_STREAM 0
#endif

#endif  // ARDUINO

// Convert unicode escape sequences (\u0123) to UTF-8
#ifndef ARDUINOJSON_DECODE_UNICODE
#define ARDUINOJSON_DECODE_UNICODE 1
#endif

// Reject byte sequences (both escaped and not escaped) unless they are a valid UTF-8 string
#ifndef ARDUINOJSON_VALIDATE_UTF8
#define ARDUINOJSON_VALIDATE_UTF8 1
#endif

// Skip possible UTF-8 BOM at the input start (EF BB BF)
#ifndef ARDUINOJSON_ENABLE_UTF8_BOM
#define ARDUINOJSON_ENABLE_UTF8_BOM 1
#endif

// Ignore comments in input
#ifndef ARDUINOJSON_ENABLE_COMMENTS
#define ARDUINOJSON_ENABLE_COMMENTS 0
#endif

#ifndef ARDUINOJSON_ENABLE_PROGMEM
#ifdef PROGMEM
#define ARDUINOJSON_ENABLE_PROGMEM 1
#else
#define ARDUINOJSON_ENABLE_PROGMEM 0
#endif
#endif

#ifndef ARDUINOJSON_CHAR_POINTER_IS_PROGMEM
#if ARDUINOJSON_ENABLE_PROGMEM
#define ARDUINOJSON_CHAR_POINTER_IS_PROGMEM 1
#else
#define ARDUINOJSON_CHAR_POINTER_IS_PROGMEM 0
#endif
#endif

#ifndef ARDUINOJSON_PROGMEM_PROBE_IMPLEMENTED
#if ARDUINOJSON_ENABLE_PROGMEM
#define ARDUINOJSON_PROGMEM_PROBE_IMPLEMENTED 1
#else
#define ARDUINOJSON_PROGMEM_PROBE_IMPLEMENTED 0
#endif
#endif

#ifndef ARDUINOJSON_ENABLE_ALIGNMENT
#ifdef ARDUINO_ARCH_AVR
// alignment isn't needed for 8-bit AVR
#define ARDUINOJSON_ENABLE_ALIGNMENT 0
#else
// but most processors need pointers to be align on word size
#define ARDUINOJSON_ENABLE_ALIGNMENT 1
#endif
#endif

// Control the exponentiation threshold for big numbers
// CAUTION: cannot be more that 1e9 !!!!
#ifndef ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD
#define ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD 1e7
#endif

// Control the exponentiation threshold for small numbers
#ifndef ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD
#define ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD 1e-5
#endif

#if ARDUINOJSON_USE_LONG_LONG && ARDUINOJSON_USE_INT64
#error ARDUINOJSON_USE_LONG_LONG and ARDUINOJSON_USE_INT64 cannot be set together
#endif
