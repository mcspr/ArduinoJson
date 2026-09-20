#pragma once

#include "../TypeTraits/And.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsBaseInstantiationOf.hpp"
#include "../TypeTraits/IsChar.hpp"
#include "../TypeTraits/IsPointer.hpp"
#include "../TypeTraits/Not.hpp"
#include "../TypeTraits/RemovePointer.hpp"

#include <memory>
#include <utility>

namespace ArduinoJson {
namespace Internals {

// String object proxy / holder
// .get() always returns an ephemeral reference

// Only expected to be used for user-provided data
// String dispatch and storage happens elsewhere

// generic containers like std::string constructed in-place

template <typename TString>
class StringRefWrapper {
 public:
  typedef const TString& ref_type;
  typedef TString string_type;

  StringRefWrapper() = default;
  explicit StringRefWrapper(TString str) :
    _str(std::move(str))
  {}

  StringRefWrapper(const StringRefWrapper&) = delete;
  StringRefWrapper& operator=(const StringRefWrapper&) = delete;

  StringRefWrapper(StringRefWrapper&&) = default;
  StringRefWrapper& operator=(StringRefWrapper&&) = default;

  ref_type get() const {
    return _str;
  }

  operator ref_type() const {
    return _str;
  }

 private:
  TString _str;
};

// generic containers like std::string passed by ref

template <typename TString>
class StringRefWrapper<const TString&> {
 public:
  typedef const TString& ref_type;
  typedef const TString& string_type;

  StringRefWrapper() = delete;
  explicit StringRefWrapper(const TString& ref) noexcept :
    _ptr(std::addressof(ref))
  {}

  StringRefWrapper(const StringRefWrapper&) = default;
  StringRefWrapper& operator=(const StringRefWrapper&) = default;

  StringRefWrapper(StringRefWrapper&&) = default;
  StringRefWrapper& operator=(StringRefWrapper&&) = default;

  ref_type get() const {
    return *_ptr;
  }

  operator ref_type() const {
    return get();
  }

 private:
  const TString* _ptr;
};

// various non-char pointers passed by value, allowed to be copied

template <typename TString>
class StringRefWrapper<const TString*> {
 public:
  typedef const TString* ref_type;
  typedef const TString* string_type;

  StringRefWrapper() = delete;
  explicit StringRefWrapper(const TString* ptr) noexcept :
    _ptr(ptr)
  {}

  StringRefWrapper(const StringRefWrapper&) = default;
  StringRefWrapper& operator=(const StringRefWrapper&) = default;

  StringRefWrapper(StringRefWrapper&&) = default;
  StringRefWrapper& operator=(StringRefWrapper&&) = default;

  ref_type get() const {
    return _ptr;
  }

  operator ref_type() const {
    return get();
  }

 private:
  const TString* _ptr;
};

// by default, lose qualifiers before generating template instances

template <typename T>
struct StringRefReference {
  typedef T type;
};

template <typename T>
struct StringRefReference<T&&> {
  typedef T type;
};

template <typename T>
struct StringRefReference<T&> {
  typedef const T &type;
};

template <typename T>
struct StringRefReference<const T&> {
  typedef const T &type;
};

template <typename T, typename = void>
struct StringRefType {
  typedef StringRefWrapper<T> type;
};

template <typename T>
struct StringRefType<StringRefWrapper<T>> {
  typedef T type;
};

template <typename T>
struct IsStringRefInstance : public IsBaseInstantiationOf<StringRefWrapper, T>::type {
};

template <typename T>
struct StringRefType<T, typename EnableIf<IsStringRefInstance<T>::value>::type> {
  typedef T type;
};

template <typename TString, typename = void>
struct StringRefWrapperHelper {
  typedef TString raw_string_type;
  typedef typename StringRefReference<TString>::type string_type;
  typedef typename StringRefType<string_type>::type wrapper_type;
};

template <typename TString>
using StringRefPointerType = typename RemovePointer<typename RemoveReference<TString>::type>::type;

template <typename TString>
struct StringRefWrapperHelper<TString,
  typename EnableIf<
    And<IsPointer<typename RemoveReference<TString>::type>,
        Not<IsChar<StringRefPointerType<TString>>>>::value>::type> {

  typedef typename RemoveReference<TString>::type raw_string_type;
  typedef const StringRefPointerType<raw_string_type>* string_type;
  typedef StringRefWrapper<string_type> wrapper_type;
};

template <typename T>
typename StringRefWrapperHelper<T>::wrapper_type MakeStringRef(T&& ref) {
  return typename StringRefWrapperHelper<T>::wrapper_type(std::forward<T>(ref));
}

}  // namespace Internals
}  // namespace ArduinoJson
