// MIT License

#pragma once

#include "../TypeTraits/And.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsChar.hpp"
#include "../TypeTraits/IsPointer.hpp"
#include "../TypeTraits/Not.hpp"
#include "../TypeTraits/RemoveConst.hpp"
#include "../TypeTraits/RemovePointer.hpp"

#include "../JsonBuffer.hpp"

#include "../StringTraits/StringTraitsBase.hpp"
#include "../Strings/Strings.hpp"

#include "../JsonSpan.hpp"

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
  static constexpr size_t Operator(SizedStringView<T> view) {
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

  static bool Operator(SizedStringView<T> view, size_t view_length, const char* other, size_t other_length) {
    return Strings::Equals::Operator(view.data(), view_length, other, other_length);
  }
};

template <typename T>
struct SizedStringViewIsNull {
  static constexpr bool Operator(SizedStringView<T> view) {
    return view.is_null();
  }
};

template <typename T>
struct SizedStringViewReference {
  static constexpr const char* Operator(SizedStringView<T> view) {
    return StringTraits<T*>::Reference::Operator(view.data());
  }
};

template <typename T>
struct SizedStringViewCopy {
  static void Operator(void* out, SizedStringView<T> view, size_t length) {
    Strings::Copy::Operator(out, view.data(), length);
  }

  static void Operator(void* out, SizedStringView<T> view) {
    Strings::Copy::Operator(out, view.data());
  }
};

template <typename T>
struct SizedStringViewDuplicate {
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
  using Copy = SizedStringViewCopy<T>;
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
struct UnsizedStringViewCopy {
  static void Operator(void* out, UnsizedStringView<T> view, size_t length) {
    Strings::Copy::Operator(out, view.data(), length);
  }

  static void Operator(void* out, UnsizedStringView<T> view) {
    Strings::Copy::Operator(out, view.data());
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

  static bool Operator(UnsizedStringView<T> view, size_t view_length, const char* other, size_t len) {
    return Strings::Equals::Operator(view.data(), view_length, other, len);
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
  using Copy = UnsizedStringViewCopy<T>;
  using Equals = UnsizedStringViewEquals<T>;
  using IsNull = UnsizedStringViewIsNull<T>;
  using Reference = UnsizedStringViewReference<T>;
};

template <typename TChar>
struct StringTraitsImpl<UnsizedStringView<TChar>, void> :
  UnsizedStringViewTraits<TChar> {
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
