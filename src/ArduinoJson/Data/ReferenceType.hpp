// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../TypeTraits/IsBaseOf.hpp"

namespace ArduinoJson {
namespace Internals {

// A type that is meant to be used by reference only (JsonArray and JsonObject)
class ReferenceType {
 public:
  bool operator==(const ReferenceType& other) const {
    // two JsonArray are equal if they are the same instance
    // (we don't compare the content)
    return this == &other;
  }

  bool operator!=(const ReferenceType& other) const {
    return this != &other;
  }
};

template <typename T>
struct IsReferenceType : IsBaseOf<ReferenceType, T>::type {
};

template <typename T>
struct IsReferenceType<T&> : IsBaseOf<ReferenceType, T>::type {
};

template <typename T>
struct IsReferenceType<const T&> : IsBaseOf<ReferenceType, T>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
