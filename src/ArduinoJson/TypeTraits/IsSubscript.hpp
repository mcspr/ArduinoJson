// MIT License

#pragma once

#include "IsBaseOf.hpp"

namespace ArduinoJson {
namespace Internals {

class JsonSubscriptTag {};

template <typename T>
struct IsSubscript : IsBaseOf<JsonSubscriptTag, T>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
