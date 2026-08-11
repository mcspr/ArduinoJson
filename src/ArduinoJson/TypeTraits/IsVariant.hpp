// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "IsBaseOf.hpp"

namespace ArduinoJson {
namespace Internals {

struct JsonVariantTag {
};

template <typename T>
struct IsVariant : IsBaseOf<JsonVariantTag, T>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
