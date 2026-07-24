// MIT License

#pragma once

#include "../Configuration.hpp"

#include "../TypeTraits/IsChar.hpp"
#include "../TypeTraits/IsPointer.hpp"
#include "../TypeTraits/IsInstantiationOf.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/RemoveConst.hpp"
#include "../TypeTraits/RemoveReference.hpp"
#include "../TypeTraits/RemoveExtent.hpp"
#include "../TypeTraits/RemovePointer.hpp"
#include "../TypeTraits/Not.hpp"
#include "../TypeTraits/Or.hpp"
#include "../TypeTraits/And.hpp"

#include "../StringTraits/StringTraitsBase.hpp"
#include "../Strings/Strings.hpp"

#include "../JsonSpan.hpp"
#include "StringRef.hpp"

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

// generalize raw char pointers / char arrays refs through internal span
// specifically for T[], bind size to runtime value instead of type info

template <typename TChar>
class BaseStringView : public JsonSpan<TChar> {
 public:
  template <typename T, size_t Size,
    typename EnableIf<
      IsSame<typename RemoveConst<T>::type,
             typename RemoveConst<TChar>::type>::value>::type* = nullptr>
  constexpr BaseStringView(T (&buf)[Size]) :
    JsonSpan<TChar>(&buf[0], Size - 1)
  {}

  template <typename T, size_t Size,
    typename EnableIf<
      And<Not<IsSame<typename RemoveConst<T>::type, typename RemoveConst<TChar>::type>>,
          IsChar<typename RemoveConst<T>::type>>::value>::type* = nullptr>
  BaseStringView(T (&buf)[Size]) :
    JsonSpan<TChar>(reinterpret_cast<TChar*>(&buf[0]), Size - 1)
  {}

  template <typename T,
    typename EnableIf<And<IsPointer<T>, IsChar<typename RemovePointer<T>::type>>::value>::type* = nullptr>
  constexpr BaseStringView(T ptr, size_t ptr_size) :
    JsonSpan<TChar>(reinterpret_cast<TChar*>(ptr), ptr_size)
  {}

  BaseStringView(const BaseStringView&) = default;
  BaseStringView& operator=(const BaseStringView&) = default;

  BaseStringView(BaseStringView&&) = default;
  BaseStringView& operator=(BaseStringView&&) = default;
};

// T[] or explicitly created T* w/ size arg

template <typename TChar>
class SizedStringView : public BaseStringView<TChar> {
 public:
  using JsonSpan<TChar>::data;
  using JsonSpan<TChar>::size;

  using BaseStringView<TChar>::BaseStringView;

  constexpr size_t length() const {
    return size();
  }

  constexpr operator TChar*() const {
    return data();
  }

  operator TChar*() {
    return data();
  }
};

template <typename T>
struct SizedStringViewLength {
  static size_t Operator(SizedStringView<T> view) {
    return view.length();
  }
};

template <typename T>
struct SizedStringViewEquals {
  static bool Operator(SizedStringView<T> view, const char* other) {
    return Strings::Equals::Operator(view.data(), view.length(), other);
  }

  static bool Operator(SizedStringView<T> view, const char* other, size_t other_length) {
    return Strings::Equals::Operator(view.data(), view.length(), other, other_length);
  }
};

template <typename T>
struct SizedStringViewIsNull {
  static bool Operator(SizedStringView<T> view) {
    return view.is_null();
  }
};

template <typename T>
struct SizedStringViewReference {
  static const char* Operator(SizedStringView<T> view) {
    return StringTraits<T*>::Reference::Operator(view.data());
  }
};

template <typename T>
struct SizedStringViewDuplicate {
  static void Operator(void* dup, SizedStringView<T> view, size_t length) {
    Strings::Duplicate::Operator(dup, view.data(), length);
  }

  static void Operator(void* dup, SizedStringView<T> view) {
    Strings::Duplicate::Operator(dup, view.data());
  }

  static const char* Operator(JsonBuffer *buffer, SizedStringView<T> view) {
    return Strings::Duplicate::Operator(buffer, view.data(), view.size());
  }
};

template <typename T, typename = void>
struct SizedStringViewBaseTraits : StringTraits<T*> {
};

template <typename T>
struct SizedStringViewBaseTraits<T,
  typename EnableIf<ShouldDuplicate<StringTraits<T*>>::value>::type> : StringTraits<T*> {

  using Duplicate = SizedStringViewDuplicate<T>;
};

template <typename T>
struct SizedStringViewTraits : SizedStringViewBaseTraits<T> {
  using Length = SizedStringViewLength<T>;
  using Equals = SizedStringViewEquals<T>;
  using IsNull = SizedStringViewIsNull<T>;
  using Reference = SizedStringViewReference<T>;
};

template <typename TChar>
struct StringTraitsImpl<SizedStringView<TChar>, void> :
  SizedStringViewTraits<TChar> {
};

// generic T* provided as values

template <typename TChar>
class UnsizedStringView : public BaseStringView<TChar> {
 public:
  using JsonSpan<TChar>::data;

  template <typename T,
    typename EnableIf<
      And<IsPointer<T>,
          IsSame<typename RemovePointer<T>::type,
                 typename RemoveConst<TChar>::type>>::value>::type* = nullptr>
  constexpr UnsizedStringView(T ptr) :
    BaseStringView<TChar>(ptr, 0)
  {}

  template <typename T,
    typename EnableIf<
      And<IsPointer<T>,
          Not<IsSame<typename RemovePointer<T>::type, typename RemoveConst<TChar>::type>>,
          IsChar<typename RemovePointer<T>::type>>::value>::type* = nullptr>
  UnsizedStringView(T ptr) :
    BaseStringView<TChar>(ptr, 0)
  {}

  constexpr operator TChar*() const {
    return data();
  }

  operator TChar*() {
    return data();
  }

 private:
  using BaseStringView<TChar>::BaseStringView;
  using JsonSpan<TChar>::size;
};

template <typename T>
struct UnsizedStringViewLength {
  static size_t Operator(UnsizedStringView<T> view) {
    return Strings::Length::Operator(view.data());
  }
};

template <typename T>
struct UnsizedStringViewEquals {
  static bool Operator(UnsizedStringView<T> view, const char* other) {
    return Strings::Equals::Operator(view.data(), other);
  }

  static bool Operator(UnsizedStringView<T> view, const char* other, size_t len) {
    return Strings::Equals::Operator(view.data(), other, len);
  }
};

template <typename T>
struct UnsizedStringViewIsNull {
  static bool Operator(UnsizedStringView<T> view) {
    return view.is_null();
  }
};

template <typename T>
struct UnsizedStringViewReference {
  static const char* Operator(UnsizedStringView<T> view) {
    return StringTraits<T*>::Reference::Operator(view.data());
  }
};

template <typename T>
struct UnsizedStringViewDuplicate {
  static void Operator(void* dup, UnsizedStringView<T> view, size_t length) {
    Strings::Duplicate::Operator(dup, view.data(), length);
  }

  static void Operator(void* dup, UnsizedStringView<T> view) {
    Strings::Duplicate::Operator(dup, view.data());
  }

  static const char* Operator(JsonBuffer* buffer, UnsizedStringView<T> view) {
    return Strings::Duplicate::Operator(buffer, view.data());
  }
};

template <typename T, typename = void>
struct UnsizedStringViewBaseTraits : StringTraits<T*> {
};

template <typename T>
struct UnsizedStringViewBaseTraits<T,
  typename EnableIf<ShouldDuplicate<StringTraits<T*>>::value>::type> : StringTraits<T*> {

  using Duplicate = UnsizedStringViewDuplicate<T>;
};

template <typename T>
struct UnsizedStringViewTraits : UnsizedStringViewBaseTraits<T> {
  using Length = UnsizedStringViewLength<T>;
  using Equals = UnsizedStringViewEquals<T>;
  using IsNull = UnsizedStringViewIsNull<T>;
  using Reference = UnsizedStringViewReference<T>;
};

template <typename TChar>
struct StringTraitsImpl<UnsizedStringView<TChar>, void> :
  UnsizedStringViewTraits<TChar> {
};

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
struct StringTraitsImpl<StringRefWrapper<TString>> :
  StringTraits<TString> {
};

} // namespace Internals

template <typename T>
Internals::UnsizedStringView<T> MakeStringView(T* str) {
  return Internals::UnsizedStringView<T>(str);
}

template <typename T>
Internals::SizedStringView<T> MakeStringView(T* str, size_t size) {
  return Internals::SizedStringView<T>(str, size);
}

}
