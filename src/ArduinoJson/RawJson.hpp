// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Data/StringRef.hpp"
#include "StringTraits/StringTraitsBase.hpp"

#include "TypeTraits/IsInstantiationOf.hpp"
#include "TypeTraits/Not.hpp"

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

template <typename String, typename = void>
struct RawStringTraitsImpl : StringTraits<String> {
};

template <typename String>
struct RawStringTraitsImpl<String,
  typename EnableIf<ShouldDuplicate<StringTraits<String>>::value>::type> : StringTraits<String> {

  typedef typename StringTraits<String>::Duplicate Duplicate;
};

template <typename String>
struct StringTraitsImpl<RawJsonString<String>> :
  RawStringTraitsImpl<String> {
};

template <typename T>
using IsRawJsonInstance =
  typename Internals::IsInstantiationOf<RawJsonString, typename RemoveConstReference<T>::type>;

template <typename T>
using RawJsonStringType =
  typename Internals::StringRefWrapperHelper<T>::string_type;

}  // namespace Internals

// by default, just forward to StringRef through the helper type ref constructor
template <typename T,
  typename Internals::EnableIf<
    Internals::Not<Internals::IsRawJsonInstance<T>>::value>::type* = nullptr>
inline Internals::RawJsonString<typename Internals::RawJsonStringType<T>> RawJson(T&& str) {
  return Internals::RawJsonString<typename Internals::RawJsonStringType<T>>(std::forward<T>(str));
}

// special case to prevent RawJson(RawJson(...)) nesting
template <typename T,
  typename Internals::EnableIf<Internals::IsRawJsonInstance<T>::value>::type* = nullptr>
inline T RawJson(T&& str) {
  return str;
}
}  // namespace ArduinoJson
