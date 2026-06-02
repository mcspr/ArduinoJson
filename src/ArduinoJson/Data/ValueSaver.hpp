// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../JsonBuffer.hpp"
#include "../JsonVariant.hpp"
#include "../StringTraits/StringTraits.hpp"
#include "../TypeTraits/EnableIf.hpp"
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

// source and destination are strings (i.e. should_duplicate present from specialization)
template <typename Source>
struct ValueSaverImpl<
    Source, typename EnableIf<StringTraits<Source>::should_duplicate>::type> {

  // output dup'ed to JsonBuffer, or stored in the object directly
  template <typename T>
  struct TDuplicate {
    typedef const char* Type;
  };

  // same as above, but convert into RawJson<JsonString>
  template <typename TString>
  struct TDuplicate<Internals::RawJsonString<TString>> {
    typedef Internals::RawJsonString<const char*> Type;
  };

  template <typename Destination>
  static bool save(JsonBuffer* buffer, Destination& dst, const Source& src) {
    if (!StringTraits<Source>::is_null(src)) {
      auto dup = StringTraits<Source>::duplicate(src, buffer);
      if (dup) {
        dst = typename TDuplicate<Source>::Type(dup);
        return true;
      }
    }

    return false;
  }
};

// const char*, const signed char*, const unsigned char*
template <typename Char>
struct ValueSaverImpl<
    Char*, typename EnableIf<!StringTraits<Char*>::should_duplicate>::type> {
  template <typename Destination>
  static bool save(JsonBuffer*, Destination& dst, Char* src) {
    dst = reinterpret_cast<const char*>(src);
    return true;
  }
};

template <typename Source,
  typename TNoCref = typename RemoveReference<Source>::type,
  typename TBase = ValueSaverImpl<TNoCref>>
struct ValueSaver : public TBase {
};

}  // namespace Internals
}  // namespace ArduinoJson
