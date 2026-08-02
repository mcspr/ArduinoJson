// MIT License

#pragma once

#include "Polyfills/attributes.hpp"

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

template <typename TImpl>
class JsonImplicitConversions {
 public:
  template <typename T>
  ARDUINOJSON_FORCE_INLINE operator T() const {
    return impl()->template as<T>();
  }

  // Avoid empty T* being used as an output type
  // Triggers substitution match earlier than through EnableIf
  operator std::nullptr_t() const = delete;

 private:
  const TImpl* impl() const {
    return static_cast<const TImpl *>(this);
  }

  TImpl* impl() {
    return static_cast<TImpl *>(this);
  }
};

}  // namespace Internals
}  // namespace ArduinoJson
