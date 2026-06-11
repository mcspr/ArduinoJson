// MIT License

#pragma once

#include "../TypeTraits/Constant.hpp"
#include "../TypeTraits/VoidType.hpp"
#include "../TypeTraits/RemoveConstReference.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsArray.hpp"

namespace ArduinoJson {
namespace Internals {

struct StringTraitsTag {
};

// type can be constructed from cstring and allows appending char & cstring by value

template <typename T, typename = void>
struct HasAppend : FalseType {
};

template <typename T>
struct HasAppend<T, VoidType<typename T::Append>> : TrueType {
};

// type can be compared internally with any other cstring

template <typename T, typename = void>
struct HasEquals : FalseType {
};

template <typename T>
struct HasEquals<T, VoidType<typename T::Equals>> : TrueType {
};

// type should be copied (duplicated) to the internal jsonbuffer before being used

template <typename T, typename = void>
struct ShouldDuplicate : FalseType {
};

template <typename T>
struct ShouldDuplicate<T, VoidType<typename T::Duplicate>> : TrueType {
};

// opposite of duplicate, type could return a cstring pointer

template <typename T, typename = void>
struct CanReference : FalseType {
};

template <typename T>
struct CanReference<T, VoidType<typename T::Reference>> : TrueType {
};

// type may be null and should be checked before being used

template <typename T, typename = void>
struct IsNullable : FalseType {
};

template <typename T>
struct IsNullable<T, VoidType<typename T::IsNull>> : TrueType {
};

// base class does not implement anything

template <typename TString, typename = void>
struct StringTraitsImpl {
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

