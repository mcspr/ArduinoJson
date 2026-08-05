// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Data/JsonVariantAs.hpp"
#include "Data/JsonFloat.hpp"

#include "TypeTraits/EnableIf.hpp"
#include "TypeTraits/IsIntegral.hpp"
#include "TypeTraits/IsSame.hpp"

#include "TypeTraits/And.hpp"
#include "TypeTraits/Not.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename TImpl>
class JsonVariantOr {
 public:
  // Returns the default value if the JsonVariant is undefined of incompatible
  template <typename T>
  typename EnableIf<!IsIntegral<T>::value, T>::type operator||(T &&defaultValue) const {
    if (impl()->template is<T>())
      return impl()->template as<T>();
    return defaultValue;
  }

  // Returns the default value if the JsonVariant is undefined of incompatible
  // Special case for string: null is treated as undefined
  const char *operator||(const char *defaultValue) const {
    const char *value = impl()->template as<const char *>();
    return value ? value : defaultValue;
  }

  // Returns the default value if the JsonVariant is undefined of incompatible
  // Special case for integers: we also accept floating point
  template <typename Integral>
  typename EnableIf<And<IsIntegral<Integral>,
                    Not<IsSame<Integral, bool>>>::value, Integral>::type
    operator||(Integral defaultValue) const {
    if (impl()->template is<Internals::JsonFloat>())
      return impl()->template as<Integral>();
    return defaultValue;
  }

  template <typename T>
  typename EnableIf<IsSame<T, bool>::value, bool>::type operator||(
      T defaultValue) const {
    if (impl()->template is<bool>())
      return impl()->template as<bool>();
    return defaultValue;
  }

 private:
  const TImpl *impl() const {
    return static_cast<const TImpl *>(this);
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
