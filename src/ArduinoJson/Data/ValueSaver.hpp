// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../JsonBuffer.hpp"
#include "../RawJson.hpp"

#include "../TypeTraits/And.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/Not.hpp"
#include "../TypeTraits/RemoveConstReference.hpp"
#include "../TypeTraits/RemoveReference.hpp"

#include "../StringTraits/StringTraits.hpp"

namespace ArduinoJson {

// Forward declarations
class JsonVariant;

namespace Internals {
namespace ValueSaverImpl {

// traits prefer non-nullable string-like types
template <typename T, typename = void>
struct IsNull {
  static bool Operator(T) {
    return false;
  }
};

// but making sure those are checked through the traits impl
template <typename T>
struct IsNull<T,
  typename EnableIf<IsNullable<StringTraits<T>>::value>::type> {

  static bool Operator(T src) {
    return StringTraits<T>::IsNull::Operator(src);
  }
};

template <typename T>
static inline bool isNull(T src) {
  return IsNull<T>::Operator(std::move(src));
}

// source is a string-like type that does not duplicate (aka is a view)
// const char*, const signed char*, const unsigned char*
// const char[], const signed char[], const unsigned char[]
template <typename T>
struct IsView
  : And<HasStringTraitsTag<T>,
        CanReference<T>,
        Not<ShouldDuplicate<T>>>::type {
};

// output dup'ed to JsonBuffer, or stored in the object directly
template <typename T, typename = void>
struct DuplicateTraits {
  using is_raw_json = FalseType;
  using string_traits = StringTraits<T>;
  using type = T;
  using duplicate_type = const char*;
};

template <typename T>
struct DuplicateTraits<T,
    typename EnableIf<IsRawJsonInstance<T>::value>::type> {

  using is_raw_json = TrueType;
  using string_traits = StringTraits<T>;
  using type = T;
  using duplicate_type =
    Internals::RawJsonString<typename DuplicateTraits<typename type::ref_type>::duplicate_type>;
};

template <typename T, typename TImpl>
struct Duplicate {
  static const char* Operator(JsonBuffer* buf, T src, size_t len) {
    void* dup = nullptr;

    if (!isNull(MakeStringRef(src.get()))) {
      dup = buf->alloc(len + 1);
      if (dup != nullptr) {
        TImpl::Copy::Operator(dup, std::move(src), len);
        reinterpret_cast<char *>(dup)[len] = '\0';
      }
    }

    return static_cast<const char*>(dup);
  }

  static const char* Operator(JsonBuffer* buf, T src) {
    const auto length = TImpl::Length::Operator(MakeStringRef(src.get()));
    return Operator(buf, std::move(src), length);
  }
};

// generic case. attempts to allocate and assigns resulting copy to the dst
// this is usually just a pointer
template <typename Destination, typename Source>
bool duplicate(JsonBuffer* buf, Destination& dst, Source src) {
  using value_duplicate_traits = DuplicateTraits<Source>;
  using value_duplicate_type =
    typename value_duplicate_traits::duplicate_type;
  using value_duplicate =
    typename value_duplicate_traits::string_traits::Duplicate;

  using make_duplicate = Duplicate<Source, value_duplicate>;
  auto* dup = make_duplicate::Operator(buf, src);
  if (dup)
    dst = value_duplicate_type(dup);

  return dup != nullptr;
}

// prepare a special overload for JsonVariant, managing one of two ways that dst could store the value
// - through the inline string, w/o allocating anything and stored directly in the variant object
// - through the JsonBuffer manual allocation i.e. though Duplicate above
template <typename Source>
bool duplicate(JsonBuffer*, JsonVariant& dst, Source src);

// generic case just assigns to the dst
template <typename Source, typename = void>
struct Save {
  template <typename Destination>
  static bool Operator(JsonBuffer*, Destination& dst, Source src) {
    dst = src;
    return true;
  }
};

// source and destination are strings (i.e. Duplicate present from specialization)
template <typename Source>
struct Save<
    Source, typename EnableIf<ShouldDuplicate<StringTraits<Source>>::value>::type> {
  template <typename Destination>
  static bool Operator(JsonBuffer* buf, Destination& dst, Source src) {
    return duplicate(buf, dst, std::move(src));
  }
};

template <typename Source>
struct Save<Source, typename EnableIf<
    IsView<StringTraits<Source>>::value>::type> {

  using source_duplicate_traits = DuplicateTraits<Source>;
  using duplicate_type =
    typename source_duplicate_traits::duplicate_type;
  using make_reference =
    typename source_duplicate_traits::string_traits::Reference;

  template <typename Destination>
  static bool Operator(JsonBuffer*, Destination& dst, Source src) {
    if (!isNull(MakeStringRef(src.get()))) {
      dst = duplicate_type(make_reference::Operator(std::move(src)));
      return true;
    }

    return false;
  }
};

// value saver implementation binds only to a T, strip T&, T&& and / or const qualifiers
// *before* using the type to avoid falling back to the generic saver impl

template <typename Source, typename = void>
struct Traits {
  using raw_source_type = Source;
  using source_type = typename RemoveConstReference<Source>::type;
  using type = Save<source_type>;
};

template <typename Source>
struct Traits<Source,
  typename EnableIf<IsArray<typename RemoveReference<Source>::type>::value>::type> {

  using raw_source_type = Source;
  using source_type = typename RemoveReference<Source>::type;
  using type = Save<source_type>;
};

}  // namespace ValueSaverImpl

template <typename Source>
struct ValueSaver :
  public ValueSaverImpl::Traits<Source>::type {

  using type = typename ValueSaverImpl::Traits<Source>::type;
};

template <typename Destination, typename Source>
inline bool saveValue(JsonBuffer* buf, Destination& dst, StringRefWrapper<Source> src) {
  return ValueSaver<decltype(MakeStringRef(src.get()))>::type::Operator(buf, dst,
                                                                        MakeStringRef(src.get()));
}

template <typename Destination, typename Source>
inline bool saveValue(JsonBuffer* buf, Destination& dst, RawJsonString<Source> src) {
  return ValueSaver<decltype(RawJson(src.get()))>::type::Operator(buf, dst, RawJson(src.get()));
}

template <typename Destination, typename Source>
inline bool saveValue(JsonBuffer* buf, Destination& dst, Source&& src) {
  return ValueSaver<Source>::type::Operator(buf, dst, std::forward<Source>(src));
}

}  // namespace Internals
}  // namespace ArduinoJson
