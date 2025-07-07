// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Polyfills/attributes.hpp>
#include <ArduinoJson/Polyfills/type_traits.hpp>
#include <ArduinoJson/Variant/VariantImpl.hpp>
#include <ArduinoJson/Variant/VariantTo.hpp>
#include "JsonVariantConst.hpp"

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

// Grants access to the internal variant API
class VariantAttorney {
 public:
  template <typename TClient>
  static VariantImpl getImpl(TClient& client) {
    return client.getImpl();
  }

  template <typename TClient>
  static VariantImpl getOrCreateImpl(TClient& client) {
    return client.getOrCreateImpl();
  }
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
