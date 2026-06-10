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
  typedef const char* Type;
};

// same as above, but convert into RawJson<JsonString>
template <typename T>
struct ValueStringDuplicate<Internals::RawJsonString<T>> {
  typedef Internals::RawJsonString<const char*> Type;
};

// source and destination are strings (i.e. Duplicate present from specialization)
template <typename Source>
struct ValueSaverImpl<
    Source, typename EnableIf<ShouldDuplicate<StringTraits<Source>>::value>::type> {

  typedef Source source_type;
  typedef typename ValueStringDuplicate<Source>::Type duplicate_type;

  template <typename Destination>
  static bool save(JsonBuffer* buffer, Destination& dst, const Source& src) {
    using Duplicate = typename StringTraits<Source>::Duplicate;
    auto* dup = Duplicate::Operator(src, buffer);
    if (dup) {
      dst = typename ValueStringDuplicate<Source>::Type(dup);
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
        IsNullable<T>,
        Not<ShouldDuplicate<T>>>::type {
};

template <typename Source>
struct ValueSaverImpl<
    Source, typename EnableIf<ValueSaverNullableView<StringTraits<Source>>::value>::type> {

  typedef typename ValueStringDuplicate<Source>::Type duplicate_type;

  template <typename Destination>
  static bool save(JsonBuffer*, Destination& dst, const Source& src) {
    using IsNull = typename StringTraits<Source>::IsNull;
    if (!IsNull::Operator(src)) {
      dst = typename ValueStringDuplicate<Source>::Type(src);
      return true;
    }

    return false;
  }
};

template <typename T>
struct ValueSaverNonNullableView
  : And<IsBaseOf<StringTraitsTag, T>,
        Not<IsNullable<T>>,
        Not<ShouldDuplicate<T>>>::type {
};

template <typename Source>
struct ValueSaverImpl<
    Source, typename EnableIf<ValueSaverNonNullableView<StringTraits<Source>>::value>::type> {

  typedef typename ValueStringDuplicate<Source>::Type duplicate_type;

  template <typename Destination>
  static bool save(JsonBuffer*, Destination& dst, const Source& src) {
    dst = typename ValueStringDuplicate<Source>::Type(src);
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
