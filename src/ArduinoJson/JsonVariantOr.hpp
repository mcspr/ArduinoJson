// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Data/JsonVariantAs.hpp"
#include "Data/JsonFloat.hpp"

#include "TypeTraits/EnableIf.hpp"
#include "TypeTraits/IsIntegral.hpp"
#include "TypeTraits/IsFloatingPoint.hpp"
#include "TypeTraits/IsSame.hpp"
#include "TypeTraits/IsVariant.hpp"
#include "TypeTraits/RemoveExtent.hpp"

#include "TypeTraits/And.hpp"
#include "TypeTraits/Not.hpp"

#include "StringTraits/StringTraits.hpp"

namespace ArduinoJson {

class JsonVariant;

namespace Internals {

// Returns the default value if the JsonVariant is undefined of incompatible
template <typename TImpl>
class JsonVariantOr {
 public:
  // Special case for cstring: null is treated as undefined
  template <typename T>
  typename EnableIf<
    Or<IsSame<T, const char *>,
       IsSame<T, char *>>::value,
    const char *>::type
  operator||(T defaultValue) const {
    const char* value = impl()->template as<const char *>();
    return value != nullptr
      ? value
      : const_cast<const char *>(defaultValue);
  }

  template <typename T>
  typename EnableIf<
    And<Not<IsSame<typename RemoveReference<T>::type, const char *>>,
        Not<IsSame<typename RemoveReference<T>::type, char *>>,
        Not<IsArray<typename RemoveReference<T>::type>>,
        CanReference<StringTraits<T>>>::value,
    const char *>::type
  operator||(T&& defaultValue) const {
    if (impl()->template is<const char*>())
      return impl()->template as<const char*>();
    return static_cast<const char*>(defaultValue);
  }

  // Special case for integers, attempting float convertion first as it covers both types
  template <typename Integral>
  typename EnableIf<
    And<IsIntegral<Integral>,
        Not<IsSame<Integral, bool>>>::value,
    Integral>::type
  operator||(Integral defaultValue) const {
    if (impl()->template is<JsonFloat>())
      return impl()->template as<Integral>();
    return defaultValue;
  }

  template <typename FloatingPoint>
  typename EnableIf<
    IsFloatingPoint<FloatingPoint>::value, FloatingPoint>::type
  operator||(FloatingPoint defaultValue) const {
    if (impl()->template is<FloatingPoint>())
      return impl()->template as<FloatingPoint>();
    return defaultValue;
  }

  template <typename T>
  typename EnableIf<
    IsSame<T, bool>::value, bool>::type
  operator||(T defaultValue) const {
    if (impl()->template is<bool>())
      return impl()->template as<bool>();
    return defaultValue;
  }

  template <typename T>
  typename EnableIf<
    Or<IsSame<typename RemoveReference<T>::type, JsonObject>,
       IsSame<typename RemoveReference<T>::type, JsonArray>>::value,
    typename JsonVariantAsConst<T>::type>::type
  operator||(const T& defaultValue) const {
    if (impl()->template is<const T&>())
      return impl()->template as<const T&>();
    return defaultValue;
  }

  template <typename T>
  typename EnableIf<
    Or<IsSame<typename RemoveReference<T>::type, JsonObject>,
       IsSame<typename RemoveReference<T>::type, JsonArray>>::value,
    typename JsonVariantAs<T>::type>::type
  operator||(T& defaultValue) {
    if (impl()->template is<T&>())
      return impl()->template as<T&>();
    return defaultValue;
  }

  template <typename T>
  typename EnableIf<IsVariant<T>::value, JsonVariant>::type
  operator||(T&& defaultValue) const {
    if (impl()->success())
      return impl()->template as<JsonVariant>();
    return defaultValue;
  }

 private:
  const TImpl *impl() const {
    return static_cast<const TImpl *>(this);
  }

  TImpl *impl() {
    return static_cast<TImpl *>(this);
  }
};

}  // namespace Internals
}  // namespace ArduinoJson
