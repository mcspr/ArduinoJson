// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Serialization/Writers/DummyWriter.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <template <typename> class TSerializer>
size_t measure(ArduinoJson::JsonVariantConst source) {
  DummyWriter dp;
  auto impl = VariantAttorney::getImpl(source);
  TSerializer<DummyWriter> serializer(dp, impl.resources());
  return impl.accept(serializer);
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
