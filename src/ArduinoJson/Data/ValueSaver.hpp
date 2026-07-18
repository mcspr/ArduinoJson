// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../JsonBuffer.hpp"
#include "../RawJson.hpp"

#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/RemoveConstReference.hpp"
#include "../TypeTraits/RemoveReference.hpp"
#include "../TypeTraits/And.hpp"
#include "../TypeTraits/Not.hpp"

#include "../StringTraits/StringTraits.hpp"

namespace ArduinoJson {

// Forward declarations
class JsonVariant;

namespace Internals {

// generic case just assigns to the dst
template <typename Source, typename = void>
struct ValueSaverImpl {
  template <typename Destination>
  static bool save(JsonBuffer*, Destination& dst, Source src) {
    dst = src;
    return true;
  }
};

// valuesaver prefers non-nullable string-like types
template <typename T, typename = void>
struct ValueSaverIsNull {
  static bool Operator(T) {
    return false;
  }
};

// but making sure those are checked through the traits impl
template <typename T>
struct ValueSaverIsNull<T,
  typename EnableIf<IsNullable<StringTraits<T>>::value>::type> {

  static bool Operator(T src) {
    return StringTraits<T>::IsNull::Operator(src);
  }
};

// output dup'ed to JsonBuffer, or stored in the object directly
template <typename T, typename = void>
struct ValueStringDuplicate {
  using is_raw_json = FalseType;
  using string_traits = StringTraits<T>;
  using type = T;
  using duplicate_type = const char*;
};

template <typename T>
struct ValueStringDuplicate<T,
    typename EnableIf<IsRawJsonInstance<T>::value>::type> {

  using is_raw_json = TrueType;
  using string_traits = StringTraits<T>;
  using type = T;
  using duplicate_type =
    Internals::RawJsonString<typename ValueStringDuplicate<typename type::ref_type>::duplicate_type>;
};

// generic case. attempts to allocate and assigns resulting copy to the dst
template <typename Destination, typename Source>
bool valueSaverDuplicate(JsonBuffer* buffer, Destination& dst, Source src) {
  using value_string_type = ValueStringDuplicate<Source>;

  using duplicate_type = typename value_string_type::duplicate_type;
  using make_duplicate = typename value_string_type::string_traits::Duplicate;

  auto* dup = make_duplicate::Operator(buffer, src);
  if (dup) {
    dst = duplicate_type(dup);
    return true;
  }

  return false;
}

// special case for jsonvariant, dispatching one of two ways that variant could duplicate the value
// - through the inline string, w/o allocating anything
// - through the jsonbuffer allocation
template <typename Source>
bool valueSaverDuplicate(JsonBuffer* buffer, JsonVariant& dst, Source src);

// source and destination are strings (i.e. Duplicate present from specialization)
template <typename Source>
struct ValueSaverImpl<
    Source, typename EnableIf<ShouldDuplicate<StringTraits<Source>>::value>::type> {
  template <typename Destination>
  static bool save(JsonBuffer* buffer, Destination& dst, Source src) {
    return valueSaverDuplicate(buffer, dst, std::move(src));
  }
};

// source is a string-like type that does not duplicate (aka is a view)
// const char*, const signed char*, const unsigned char*
// const char[], const signed char[], const unsigned char[]
template <typename T>
struct ValueSaverView
  : And<HasStringTraitsTag<T>,
        CanReference<T>,
        Not<ShouldDuplicate<T>>>::type {
};

template <typename Source>
struct ValueSaverImpl<Source, typename EnableIf<
    ValueSaverView<StringTraits<Source>>::value>::type> {

  using value_string_type = ValueStringDuplicate<Source>;
  using duplicate_type = typename value_string_type::duplicate_type;

  using make_reference = typename value_string_type::string_traits::Reference;
  using is_null = ValueSaverIsNull<Source>;

  template <typename Destination>
  static bool save(JsonBuffer*, Destination& dst, Source src) {
    if (!is_null::Operator(src)) {
      dst = duplicate_type(make_reference::Operator(src));
      return true;
    }

    return false;
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
