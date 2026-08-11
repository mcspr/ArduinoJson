// MIT License

#pragma once

#include "Polyfills/attributes.hpp"

#include "TypeTraits/EnableIf.hpp"
#include "TypeTraits/IsFloatingPoint.hpp"
#include "TypeTraits/IsIntegral.hpp"
#include "TypeTraits/IsSame.hpp"
#include "TypeTraits/IsJsonReference.hpp"

#include "TypeTraits/And.hpp"
#include "TypeTraits/Or.hpp"

#include "Data/JsonNull.hpp"

#include "StringTraits/StringTraitsBase.hpp"
#include "StringTraits/StringTraits.hpp"

#include <cstddef>

namespace ArduinoJson {

class JsonArray;
class JsonObject;

class JsonVariant;

// *CAUTION*: IsBaseOf (e.g. IsVariant) trait checks would fail to compile w/ clang++,
// since currently neither of the types would be complete at that point of template instantiation chain
// Using template specializations when it is possible to do so
// Otherwise, using IsSame<..., ...> to dispatch specific type overloads

namespace Internals {
namespace TypeTraits {

template <typename T>
struct IsImplicitlyConvertibleType :
  Or<IsIntegral<T>,
     IsFloatingPoint<T>,
     HasStringTraits<T>>::type {
};

template <>
struct IsImplicitlyConvertibleType<JsonVariant> : TrueType {
};

template <>
struct IsImplicitlyConvertibleType<JsonNull> : TrueType {
};

}

template <typename T>
struct IsImplicitlyConvertible :
    TypeTraits::IsImplicitlyConvertibleType<T>::type {
};

template <typename T>
struct IsImplicitlyConvertible<T const> :
    TypeTraits::IsImplicitlyConvertibleType<T>::type {
};

struct JsonImplicitAnyReference {
};

struct JsonImplicitConstReference {
};

template <typename, typename>
struct IsJsonImplicitReference : FalseType {
};

template <typename T>
struct IsJsonImplicitReference<JsonImplicitAnyReference, T> :
    IsJsonReference<T>::type {
};

template <typename, typename>
struct IsJsonImplicitConstReference : FalseType {
};

template <typename T>
struct IsJsonImplicitConstReference<JsonImplicitConstReference, T> :
  IsJsonConstReference<T>::type {

};

template <typename TImpl, typename TRef = JsonImplicitAnyReference>
class JsonImplicitConversions {
 public:
  // generic conversions, previously declared via JsonVariant::as()
  template <typename T, typename EnableIf<
    IsImplicitlyConvertible<T>::value>::type* = nullptr>
  ARDUINOJSON_FORCE_INLINE operator T() const {
    return impl()->template as<T>();
  }

  // generate implicit conversion for references to reference-only types
  template <typename T, typename EnableIf<
    IsJsonImplicitReference<TRef, T>::value>::type* = nullptr>
  ARDUINOJSON_FORCE_INLINE operator T&() const {
    return impl()->template as<T&>();
  }

  // generate implicit conversion for references to reference-only types, but *only* for const qualified ones
  template <typename T, typename EnableIf<
    IsJsonImplicitConstReference<TRef, T>::value>::type* = nullptr>
  ARDUINOJSON_FORCE_INLINE operator T&() const {
    return impl()->template as<T&>();
  }

  // Avoid empty T* being used as an output type
  // Triggers substitution match earlier than through EnableIf
  operator std::nullptr_t() const = delete;

 private:
  const TImpl* impl() const {
    return static_cast<const TImpl *>(this);
  }
};

}  // namespace Internals
}  // namespace ArduinoJson
