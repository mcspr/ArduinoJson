// MIT License

#pragma once

#include "Polyfills/attributes.hpp"

#include "TypeTraits/EnableIf.hpp"
#include "TypeTraits/IsFloatingPoint.hpp"
#include "TypeTraits/IsIntegral.hpp"
#include "TypeTraits/IsPointer.hpp"
#include "TypeTraits/RemovePointer.hpp"
#include "TypeTraits/IsVariant.hpp"

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

namespace Internals {

// *CAUTION*: IsReferenceType, IsVariant, etc. trait checks would fail to compile w/ clang++,
// since currently neither of the types would be complete at that point of template instantiation chain
// Using IsSame<..., ...> when it is necessary to match specific type overloads

struct JsonImplicitAnyReference {
};

struct JsonImplicitConstReference {
};

template <typename TImpl, typename TRef = JsonImplicitAnyReference>
class JsonImplicitConversions {
 public:
  // generic conversions, previously declared via JsonVariant::as()
  template <typename T, typename EnableIf<
    Or<IsSame<T, JsonVariant>,
       IsSame<T, JsonNull>,
       IsIntegral<T>,
       IsFloatingPoint<T>,
       HasStringTraits<T>>::value>::type* = nullptr>
  ARDUINOJSON_FORCE_INLINE operator T() const {
    return impl()->template as<T>();
  }

  // generate implicit conversion for references to reference-only types
  template <typename T, typename EnableIf<
    And<IsSame<TRef, JsonImplicitAnyReference>,
        Or<IsSame<typename RemoveConst<T>::type, JsonArray>,
           IsSame<typename RemoveConst<T>::type, JsonObject>>>::value>::type* = nullptr>
  ARDUINOJSON_FORCE_INLINE operator T&() const {
    return impl()->template as<T&>();
  }

  // generate implicit conversion for references to reference-only types, but *only* for const qualified ones
  template <typename T, typename EnableIf<
    And<IsSame<TRef, JsonImplicitConstReference>,
        Or<IsSame<T, const JsonArray>,
           IsSame<T, const JsonObject>>>::value>::type* = nullptr>
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
