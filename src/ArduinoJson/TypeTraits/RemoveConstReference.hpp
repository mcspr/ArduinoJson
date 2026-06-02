// MIT License

#pragma once

#include "RemoveConst.hpp"
#include "RemoveReference.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename T>
using RemoveConstReference = RemoveConst<typename Internals::RemoveReference<T>::type>;

}  // namespace Internals
}  // namespace ArduinoJson
