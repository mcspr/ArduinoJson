#pragma once

#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsBaseInstantiationOf.hpp"

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
  explicit StringRefWrapper(TString& ref) noexcept :
    _ref(ref)
  {}

  explicit StringRefWrapper(const TString& ref) noexcept :
    _ref(ref)
  {}

  ref_type get() const {
    return _ref;
  }

  operator ref_type() const {
    return get();
  }

 private:
  const TString& _ref;
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

template <typename T>
typename StringRefWrapperHelper<T>::wrapper_type MakeStringRef(T&& ref) {
  return typename StringRefWrapperHelper<T>::wrapper_type(std::forward<T>(ref));
}

}
}
