// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonImplicitConversions.hpp"
#include "JsonImplicitReferenceConversions.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename TImpl>
class JsonVariantCasts :
  public JsonImplicitReferenceConversions<TImpl>,
  public JsonImplicitConversions<TImpl> {

};

}  // namespace Internals
}  // namespace ArduinoJson
