#pragma once

#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/RemoveConstReference.hpp"
#include "../TypeTraits/RemoveReference.hpp"
#include "../TypeTraits/RemoveExtent.hpp"
#include "../TypeTraits/IsPointer.hpp"
#include "../TypeTraits/IsArray.hpp"

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

  StringRefWrapper() = default;
  explicit StringRefWrapper(TString str) :
    _str(std::move(str))
  {}

  ref_type get() const {
    return _str;
  }

  operator ref_type() const {
    return _str;
  }

 private:
  TString _str;
};

template <typename TString>
class StringRefWrapper<TString&&> :
  public StringRefWrapper<TString> {
};

// generic containers like std::string passed by ref

template <typename TString>
class StringRefWrapper<const TString&> {
 public:
  typedef const TString& ref_type;

  StringRefWrapper() = delete;
  explicit StringRefWrapper(TString& ref) :
    _ref(ref)
  {}

  explicit StringRefWrapper(const TString& ref) :
    _ref(ref)
  {}

  ref_type get() const {
    return _ref;
  }

  operator ref_type() const {
    return _ref;
  }

 private:
  const TString& _ref;
};

template <typename TString>
class StringRefWrapper<TString&> :
  public StringRefWrapper<const TString&> {

 public:
  using StringRefWrapper<const TString&>::StringRefWrapper;
};

// literals, stack or globals

template <typename TChar>
class StringRefWrapper<TChar*> {
 public:
  using ref_type = TChar*;

  StringRefWrapper() = default;
  explicit StringRefWrapper(TChar* str) :
    _str(str)
  {}

  template <size_t Size>
  explicit StringRefWrapper(TChar (&str)[Size]) :
    _str(&str[0])
  {}

  ref_type get() const {
    return _str;
  }

  operator ref_type() const {
    return _str;
  }

 private:
  TChar *_str{};
};

// vla is whatever

template <typename T>
class StringRefWrapper<T[]>;

// lose qualifiers before generating template instances

template <typename TString, typename = void>
struct StringRefWrapperHelper {
  typedef TString raw_string_type;
  typedef TString string_type;
  typedef StringRefWrapper<string_type> wrapper_type;
};

template <typename TString>
struct StringRefWrapperHelper<TString,
  typename EnableIf<IsPointer<typename RemoveReference<TString>::type>::value, void>::type> {

  typedef TString raw_string_type;
  typedef typename RemoveConstReference<TString>::type string_type;
  typedef StringRefWrapper<string_type> wrapper_type;
};

template <typename TString>
struct StringRefWrapperHelper<TString,
  typename EnableIf<IsArray<typename RemoveReference<TString>::type>::value, void>::type> {

  typedef TString raw_string_type;
  typedef typename RemoveExtent<typename RemoveReference<TString>::type>::type* string_type;
  typedef StringRefWrapper<string_type> wrapper_type;
};

}
}
