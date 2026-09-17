// MIT License

#pragma once

#include <ArduinoJson/Numbers/parseNumber.hpp>
#include <ArduinoJson/Numbers/isNumber.hpp>
#include <ArduinoJson/Polyfills/math.hpp>

#include <ArduinoJson/TypeTraits/IsFloatingPoint.hpp>
#include <ArduinoJson/TypeTraits/IsIntegral.hpp>
#include <ArduinoJson/TypeTraits/TypeIdentity.hpp>

#include <ostream>
#include <type_traits>

#include <cmath>

using ArduinoJson::Internals::ConvertResult;
using ArduinoJson::Internals::ParsedNumberResult;
using NumberType = ParsedNumberResult::Type;

// xxx workaround catch hpp template instantiation order in case INFO(...) is used
template <typename T>
inline std::ostream& operator<<(std::ostream&, const ConvertResult<T>&);
inline std::ostream& operator<<(std::ostream&, ParsedNumberResult);
inline std::ostream& operator<<(std::ostream&, NumberType);

#define CATCH_CONFIG_FAST_COMPILE
#include <catch.hpp>

template <typename T>
struct Catch::StringMaker<ConvertResult<T>> {
  static std::string convert(ConvertResult<T> convert) {
    if (convert)
      return std::to_string(convert.value);

    return "FAILED";
  }
};

template <typename T>
inline std::ostream& operator<<(std::ostream& out, const ConvertResult<T>& convert) {
  out << Catch::StringMaker<ConvertResult<T>>::convert(convert);
  return out;
}

template <>
struct Catch::StringMaker<NumberType> {
  // xxx: catch builtin enum registration wants explicit member names as vaargs
  static std::string convert(NumberType type) {
    switch (static_cast<std::underlying_type<NumberType>::type>(type)) {
      case 0:
        return "Invalid";

      case 1:
        return "Float";

      case 2:
        return "Double";

      case 3:
        return "Signed Integer";

      case 4:
        return "Unsigned Integer";
    }

    return "{?}";
  }
};

inline std::ostream& operator<<(std::ostream& out, NumberType type) {
  out << Catch::StringMaker<NumberType>::convert(type);
  return out;
}

template <>
struct Catch::StringMaker<ParsedNumberResult> {
  static std::string convert(const ParsedNumberResult& result) {
    std::string out;
    out += "ParsedNumberResult<";
    out += Catch::StringMaker<NumberType>::convert(result.value.type());
    out += '>';

    return out;
  }
};

inline std::ostream& operator<<(std::ostream& out, const ParsedNumberResult& result) {
  out << Catch::StringMaker<ParsedNumberResult>::convert(result);
  return out;
}

namespace {

// 5.x branch does always doesn't check for conversion validity, silently converting value to either `0` or `inf`
// which also does apply to float -> double, in case conversion result happens to only fit into larger type vs. requested one
// Numbers/parseNumber.hpp & Numbers/convertNumber.hpp backported from 7.x to reflect parsing state w/ some additional strict checks

using ArduinoJson::Internals::EnableIf;
using ArduinoJson::Internals::IsFloatingPoint;
using ArduinoJson::Internals::IsIntegral;
using ArduinoJson::Internals::TypeIdentityType;

using ArduinoJson::Internals::isNumber;
using ArduinoJson::Internals::isInfinity;
using ArduinoJson::Internals::isNaN;
using ArduinoJson::Internals::parseJsonNumber;

// nb. since catch2 macro-generated types are pretty expensive to compile, confide them into a couple of funcs instead of
// having them repeat in test-case blocks. retains source line info from the call site using internal api, *may* break on updates
#pragma push_macro("CATCH_INTERNAL_LINEINFO")
#undef CATCH_INTERNAL_LINEINFO
#define CATCH_INTERNAL_LINEINFO source_line_info

#define __CATCH_LOCAL_LINEINFO ::Catch::SourceLineInfo source_line_info = ::Catch::SourceLineInfo(__builtin_FILE(), __builtin_LINE())
#define __CATCH_NESTED_LINEINFO ::Catch::SourceLineInfo source_line_info

inline void expectNumber(const char* s, __CATCH_LOCAL_LINEINFO) {
  REQUIRE(isNumber(s));
}

#define REQUIRE_IS_NUMBER(S) expectNumber(S)

inline void expectNumberFailure(const char* s, __CATCH_LOCAL_LINEINFO) {
  REQUIRE_FALSE(isNumber(s));
}

#define REQUIRE_FALSE_IS_NUMBER(S) expectNumberFailure(S)

template <typename T, typename EnableIf<IsIntegral<T>::value>::type* = nullptr>
void expectConvertResult(ConvertResult<T> convert, T expected, __CATCH_NESTED_LINEINFO) {
  REQUIRE(convert.value == expected);
}

template <typename T, typename EnableIf<IsFloatingPoint<T>::value>::type* = nullptr>
void expectConvertResult(ConvertResult<T> convert, T expected, __CATCH_NESTED_LINEINFO) {
  if (isInfinity(convert.value) || isNaN(convert.value))
    CHECK(std::signbit(convert.value) == std::signbit(expected));

  if (isInfinity(expected)) {
    REQUIRE(isInfinity(convert.value));
    REQUIRE_FALSE(isNaN(convert.value));

  } else if (isNaN(expected)) {
    REQUIRE(isNaN(convert.value));
    REQUIRE_FALSE(isInfinity(convert.value));

  } else {
    REQUIRE(convert.value == Approx(expected));

  }
}

template <typename T>
void expectParseValue(const char* s, T expected_value, __CATCH_LOCAL_LINEINFO) {
  const auto result = parseJsonNumber(s);
  CHECK(result);
  const auto convert = result.convertTo<T>();
  expectConvertResult(convert, expected_value, source_line_info);
}

template <typename T>
void expectParseTypedValue(const char* s, NumberType expected_type, T expected_value, __CATCH_LOCAL_LINEINFO) {
  const auto result = parseJsonNumber(s);
  const auto convert = result.convertTo<T>();

  CHECK(result.value.type() == expected_type);
  expectConvertResult(convert, expected_value, source_line_info);
}

template <typename T>
void expectParseTypedConvertible(const char* s, NumberType expected_type, __CATCH_LOCAL_LINEINFO) {
  const auto result = parseJsonNumber(s);
  CHECK(result.value.type() == expected_type);
  REQUIRE(result.convertTo<T>());
}

template <typename T>
void expectParseTypedInconvertible(const char* s, NumberType expected_type, __CATCH_LOCAL_LINEINFO) {
  const auto result = parseJsonNumber(s);
  CHECK(result.value.type() == expected_type);
  REQUIRE_FALSE(result.convertTo<T>());
}

template <typename T>
void expectParseInconvertible(const char* s, __CATCH_LOCAL_LINEINFO) {
  const auto result = parseJsonNumber(s);
  CHECK(result);
  REQUIRE_FALSE(result.convertTo<T>());
}

template <typename T>
void expectParseInconvertibleFailure(const char* s, __CATCH_LOCAL_LINEINFO) {
  const auto result = parseJsonNumber(s);
  CHECK_FALSE(result);
  REQUIRE_FALSE(result.convertTo<T>());
}

template <typename T>
void expectParseConvertible(const char* s, __CATCH_LOCAL_LINEINFO) {
  const auto result = parseJsonNumber(s);
  CHECK(result);
  REQUIRE(result.convertTo<T>());
}

inline void expectParseType(const char* s, NumberType expected_type, __CATCH_LOCAL_LINEINFO) {
  const auto result = parseJsonNumber(s);
  REQUIRE(result.value.type() == expected_type);
}

inline void expectParsingFailure(const char* s, __CATCH_LOCAL_LINEINFO) {
  const auto result = parseJsonNumber(s);
  REQUIRE_FALSE(result);
}

#pragma pop_macro("CATCH_INTERNAL_LINEINFO")

// verify that string `S` can be parsed and the resulting object type is NumberType `E`
#define REQUIRE_PARSE_TYPE(E, S) expectParseType(S, E)

// verify that
// a) string `S` can be parsed
// b) resulting object type is NumberType `E`
// c) resulting object value can be converted into `T`
// d) converted value is the same as the expected `V`
#define REQUIRE_PARSE_TYPED_VALUE(T, E, S, V) expectParseTypedValue T(S, E, TypeIdentityType T{V})

// -//- but skip b)
#define REQUIRE_PARSE_VALUE(T, S, V) expectParseValue T(S, TypeIdentityType T{V})

// -//- but skip d)
#define REQUIRE_PARSE_TYPED_CONVERTIBLE(T, E, S) expectParseTypedConvertible T(S, E)

// -//- but skip b) and d)
#define REQUIRE_PARSE_CONVERTIBLE(T, S) expectParseConvertible T(S)

// c) conversion can't be performed on the specified `T`
#define REQUIRE_PARSE_TYPED_INCONVERTIBLE(T, E, S) expectParseTypedInconvertible T(S, E)

// -//- but skip b)
#define REQUIRE_PARSE_INCONVERTIBLE(T, S) expectParseInconvertible T(S)

// expect both parsing string `S` and type conversion to NumberType `T` to fail
// (and since parsing failure should never produce a valid ConvertResult<T>)
#define REQUIRE_PARSE_FAIL(T, S) expectParseInconvertibleFailure T(S)

}  // namespace
