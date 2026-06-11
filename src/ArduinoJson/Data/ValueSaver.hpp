// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../JsonBuffer.hpp"

#include "../StringTraits/StringTraits.hpp"
#include "../RawJson.hpp"

#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsBaseOf.hpp"
#include "../TypeTraits/RemoveConstReference.hpp"
#include "../TypeTraits/RemoveReference.hpp"
#include "../TypeTraits/And.hpp"
#include "../TypeTraits/Not.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename Source, typename = void>
struct ValueSaverImpl {
  typedef Source duplicate_type;

  template <typename Destination>
  static bool save(JsonBuffer*, Destination& dst, const Source& src) {
    dst = src;
    return true;
  }
};

// output dup'ed to JsonBuffer, or stored in the object directly
template <typename T>
struct ValueStringDuplicate {
  typedef T type;
  typedef const char* duplicate_type;
};

// same as above, but convert into RawJson<JsonString>
template <typename T>
struct ValueStringDuplicate<Internals::RawJsonString<T>> {
  typedef T type;
  typedef Internals::RawJsonString<const char*> duplicate_type;
};

// source and destination are strings (i.e. Duplicate present from specialization)
template <typename Source>
struct ValueSaverImpl<
    Source, typename EnableIf<ShouldDuplicate<StringTraits<Source>>::value>::type> {

  typedef ValueStringDuplicate<Source> value_string_type;
  typedef typename value_string_type::type source_type;
  typedef typename value_string_type::duplicate_type duplicate_type;

  template <typename Destination>
  static bool save(JsonBuffer* buffer, Destination& dst, const Source& src) {
    using Duplicate = typename StringTraits<source_type>::Duplicate;
    auto* dup = Duplicate::Operator(buffer, src);
    if (dup) {
      dst = duplicate_type(dup);
      return true;
    }

    return false;
  }
};

// source is a string-like type that does not duplicate (aka is a view)
// const char*, const signed char*, const unsigned char*
// const char[], const signed char[], const unsigned char[]
template <typename T>
struct ValueSaverNullableView
  : And<IsBaseOf<StringTraitsTag, T>,
        CanReference<T>,
        IsNullable<T>,
        Not<ShouldDuplicate<T>>>::type {
};

template <typename Source>
struct ValueSaverImpl<
    Source, typename EnableIf<ValueSaverNullableView<StringTraits<Source>>::value>::type> {

  typedef ValueStringDuplicate<Source> value_string_type;
  typedef typename value_string_type::duplicate_type duplicate_type;

  template <typename Destination>
  static bool save(JsonBuffer*, Destination& dst, const Source& src) {
    using traits_type = StringTraits<Source>;

    using is_null = typename traits_type::IsNull;
    if (!is_null::Operator(src)) {
      using take_reference = typename traits_type::Reference;
      dst = duplicate_type(take_reference::Operator(src));
      return true;
    }

    return false;
  }
};

template <typename T>
struct ValueSaverNonNullableView
  : And<IsBaseOf<StringTraitsTag, T>,
        CanReference<T>,
        Not<IsNullable<T>>,
        Not<ShouldDuplicate<T>>>::type {
};

template <typename Source>
struct ValueSaverImpl<
    Source, typename EnableIf<ValueSaverNonNullableView<StringTraits<Source>>::value>::type> {

  typedef ValueStringDuplicate<Source> value_string_type;
  typedef typename value_string_type::duplicate_type duplicate_type;

  template <typename Destination>
  static bool save(JsonBuffer*, Destination& dst, const Source& src) {
    using traits_type = StringTraits<Source>;
    using take_reference = typename traits_type::Reference;
    dst = duplicate_type(take_reference::Operator(src));
    return true;
  }
};

// most common use-case is instantiating this from T&&, hence it is necessary to
// strip useless qualifiers before passing through to the actual processing

template <typename Source, typename = void>
struct ValueSaverHelper {
  typedef Source raw_source_type;
  typedef typename RemoveConstReference<Source>::type source_type;
  typedef ValueSaverImpl<source_type> value_saver_type;
};

template <typename Source>
struct ValueSaverHelper<Source,
  typename EnableIf<IsArray<typename RemoveReference<Source>::type>::value, void>::type> {

  typedef Source raw_source_type;
  typedef typename RemoveReference<Source>::type source_type;
  typedef ValueSaverImpl<source_type> value_saver_type;
};

template <typename Source>
struct ValueSaver :
  public ValueSaverHelper<Source>::value_saver_type {
  typedef typename ValueSaverHelper<Source>::value_saver_type value_saver_type;
};

}  // namespace Internals
}  // namespace ArduinoJson
