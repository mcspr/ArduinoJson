// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../JsonBuffer.hpp"
#include "../StringTraits/StringTraits.hpp"

#include "../RawJson.hpp"

#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/RemoveConstReference.hpp"
#include "../TypeTraits/RemoveReference.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename Source, typename = void>
struct ValueSaverImpl {
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

// source and destination are strings (i.e. should_duplicate present from specialization)
template <typename Source>
struct ValueSaverImpl<
    Source, typename EnableIf<StringTraits<Source>::should_duplicate>::type> {

  template <typename Destination>
  static bool save(JsonBuffer* buffer, Destination& dst, const Source& src) {
    if (!StringTraits<Source>::is_null(src)) {
      auto dup = StringTraits<Source>::duplicate(src, buffer);
      if (dup) {
        dst = typename ValueStringDuplicate<Source>::Type(dup);
        return true;
      }
    }

    return false;
  }
};

// const char*, const signed char*, const unsigned char*
// const char[], const signed char[], const unsigned char[]
template <typename Source>
struct ValueSaverImpl<
    Source, typename EnableIf<!StringTraits<Source>::should_duplicate>::type> {

  template <typename Destination>
  static bool save(JsonBuffer*, Destination& dst, const Source& src) {
    if (!StringTraits<Source>::is_null(src)) {
      dst = typename ValueStringDuplicate<Source>::Type(src);
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
};

}  // namespace Internals
}  // namespace ArduinoJson
