// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "StringTraits/StringTraitsBase.hpp"
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
struct StringTraitsImpl<RawJsonString<String>, void> {
  static bool is_null(RawJsonString<String> source) {
    return StringTraits<String>::is_null(source.get());
  }

  typedef RawJsonString<const char*> duplicate_t;

  typedef RawJsonString<String> source_type;
  typedef typename source_type::ref_type ref_type;

  template <typename Buffer>
  static duplicate_t duplicate(const source_type& source, Buffer* buffer) {
    return duplicate_t(StringTraits<ref_type>::duplicate(source.get(), buffer));
  }

  static const bool has_append = false;
  static const bool has_equals = false;
  static const bool should_duplicate = StringTraits<ref_type>::should_duplicate;
};

}  // namespace Internals

template <typename T, typename Helper = Internals::StringRefWrapperHelper<T>>
inline Internals::RawJsonString<typename Helper::string_type> RawJson(T&& str) {
  return Internals::RawJsonString<typename Helper::string_type>(std::forward<T>(str));
}
}  // namespace ArduinoJson
