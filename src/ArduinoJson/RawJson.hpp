// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "StringTraits/StringTraitsBase.hpp"

#include "TypeTraits/IsInstantiationOf.hpp"
#include "TypeTraits/And.hpp"
#include "TypeTraits/Not.hpp"

#include "Data/StringRef.hpp"

#include <utility>

namespace ArduinoJson {
namespace Internals {

// A special type of data that can be used to insert pregenerated JSON portions.
template <typename T>
class RawJsonString : public StringRefWrapperHelper<T>::wrapper_type {
  typedef StringRefWrapperHelper<T> helper_type;
  typedef typename helper_type::wrapper_type wrapper_type;

 public:
  using wrapper_type::wrapper_type;
};

template <typename String>
struct StringTraitsImpl<RawJsonString<String>> : StringTraits<String> {
};

template <typename T>
struct IsRawJsonInstance : Internals::IsInstantiationOf<
    RawJsonString, typename RemoveConstReference<T>::type>::type {
};

template <typename T>
using RawJsonStringType =
  typename Internals::StringRefWrapperHelper<T>::string_type;

template <typename T>
using EnableIfNotRawJsonLike =
  EnableIf<And<Not<IsStringRefInstance<T>>,
               Not<IsRawJsonInstance<T>>>::value>;

}  // namespace Internals

// by default, just forward to StringRef through the helper type ref constructor
template <typename T,
  typename Internals::EnableIfNotRawJsonLike<T>::type* = nullptr>
inline Internals::RawJsonString<Internals::RawJsonStringType<T>> RawJson(T&& str) {
  return Internals::RawJsonString<Internals::RawJsonStringType<T>>(std::forward<T>(str));
}

// similarly, assume we'd want to clone the other ref by value type

template <typename T,
  typename Internals::EnableIf<Internals::IsStringRefInstance<T>::value>::type* = nullptr>
inline Internals::RawJsonString<typename T::string_type> RawJson(T ref) {
  return Internals::RawJsonString<typename T::string_type>(ref.get());
}

// since there is no string ref helper type present, special case to prevent RawJson(RawJson(...)) nesting
template <typename T,
  typename Internals::EnableIf<Internals::IsRawJsonInstance<T>::value>::type* = nullptr>
inline T RawJson(T&& str) {
  return str;
}

template <typename T,
  typename Internals::EnableIf<Internals::IsRawJsonInstance<T>::value>::type* = nullptr>
T MakeStringRef(T str) {
  return str;
}

}  // namespace ArduinoJson
