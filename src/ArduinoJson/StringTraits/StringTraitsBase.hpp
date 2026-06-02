// MIT License

#pragma once

#include "../TypeTraits/RemoveConstReference.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsArray.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename TString, typename = void>
struct StringTraitsImpl {
  static const bool has_append = false;
  static const bool has_equals = false;
};

// helper type to generalize impl type bindings to just `const T` or `T`

template <typename TString, typename = void>
struct StringTraitsHelper {
  typedef TString raw_string_type;
  typedef typename RemoveConstReference<TString>::type string_type;
  typedef StringTraitsImpl<string_type> traits_type;
};

// special case for arrays, to avoid interpreting them as value types

template <typename TString>
struct StringTraitsHelper<TString,
  typename EnableIf<IsArray<typename RemoveReference<TString>::type>::value, void>::type> {

  typedef TString raw_string_type;
  typedef typename RemoveReference<TString>::type string_type;
  typedef StringTraitsImpl<string_type> traits_type;
};

template <typename TString,
  typename Enable = void,
  typename Helper = StringTraitsHelper<TString>>
struct StringTraits : public Helper::traits_type {
  using helper_type = Helper;
};

}  // namespace Internals
}  // namespace ArduinoJson

