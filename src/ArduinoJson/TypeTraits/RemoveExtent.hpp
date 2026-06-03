#pragma once

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

template <typename T>
struct RemoveExtent {
  typedef T type;
};

template <typename T, size_t Size>
struct RemoveExtent<T[Size]> {
  typedef T type;
};

}  // namespace Internals
}  // namespace ArduinoJson
