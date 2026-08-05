// MIT License

#include "../Configuration.hpp"

#include "../TypeTraits/And.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsChar.hpp"
#include "../TypeTraits/IsInstantiationOf.hpp"
#include "../TypeTraits/IsPointer.hpp"
#include "../TypeTraits/Or.hpp"
#include "../TypeTraits/RemoveExtent.hpp"
#include "../TypeTraits/RemovePointer.hpp"
#include "../TypeTraits/RemoveReference.hpp"

#include "StringRef.hpp"
#include "StringView.hpp"

#pragma once

namespace ArduinoJson {
namespace Internals {

// return by value instead of default stringref behaviour

template <typename T>
class StringViewWrapper {
 public:
  using string_type = T;
  using ref_type = T;

  StringViewWrapper() = default;
  constexpr explicit StringViewWrapper(ref_type str) :
    _str(str)
  {}

  constexpr ref_type get() const {
    return _str;
  }

  constexpr operator ref_type() const {
    return get();
  }

 private:
  ref_type _str{};
};

template <typename T>
class StringRefWrapper<SizedStringView<T>> :
  public StringViewWrapper<SizedStringView<T>> {

  using StringViewWrapper<SizedStringView<T>>::StringViewWrapper;
};

template <typename T>
class StringRefWrapper<UnsizedStringView<T>> :
  public StringViewWrapper<UnsizedStringView<T>> {

  using StringViewWrapper<UnsizedStringView<T>>::StringViewWrapper;
};

// T*

template <typename TString>
using StringRefPointerType =
    typename RemovePointer<typename RemoveReference<TString>::type>::type;

template <typename TString>
struct StringRefWrapperHelper<TString,
  typename EnableIf<
    And<IsPointer<typename RemoveReference<TString>::type>,
        IsChar<StringRefPointerType<TString>>>::value>::type> {

  typedef TString raw_string_type;
  typedef UnsizedStringView<
    StringRefPointerType<TString>> string_type;
  typedef StringRefWrapper<string_type> wrapper_type;
};

// T[]

template <typename TString>
struct StringRefWrapperHelper<TString,
  typename EnableIf<IsArray<typename RemoveReference<TString>::type>::value>::type> {

  typedef TString raw_string_type;
  typedef SizedStringView<
    typename RemoveExtent<typename RemoveReference<TString>::type>::type> string_type;
  typedef StringRefWrapper<string_type> wrapper_type;
};

// already constructed

template <typename TString>
struct StringRefWrapperHelper<TString,
  typename EnableIf<Or<
    IsInstantiationOf<SizedStringView, typename RemoveReference<TString>::type>,
    IsInstantiationOf<UnsizedStringView, typename RemoveReference<TString>::type>>::value>::type> {

  typedef TString raw_string_type;
  typedef typename RemoveConstReference<TString>::type string_type;
  typedef StringRefWrapper<string_type> wrapper_type;
};

#if ARDUINOJSON_ENABLE_PROGMEM

template <typename TString>
struct StringRefWrapperHelper<TString,
  typename EnableIf<
    And<IsPointer<typename RemoveReference<TString>::type>,
        IsSame<typename RemoveConst<StringRefPointerType<TString>>::type,
               __FlashStringHelper>>::value>::type> {

  typedef TString raw_string_type;
  typedef typename RemoveConstReference<raw_string_type>::type string_type;
  typedef StringRefWrapper<string_type> wrapper_type;
};

#endif

template <typename TString>
struct StringTraitsImpl<StringRefWrapper<TString>, void> :
  StringTraits<TString> {
};

}
}
