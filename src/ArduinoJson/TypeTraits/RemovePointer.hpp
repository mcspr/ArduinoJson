// MIT License

#pragma once

namespace ArduinoJson {
namespace Internals {

// A meta-function that return the type T without the pointer
template <typename T>
struct RemovePointer {
  typedef T type;
};

template <typename T>
struct RemovePointer<T*> {
  typedef T type;
};

template <typename T>
struct RemovePointer<T* const> {
  typedef const T type;
};

template <typename T>
struct RemovePointer<T* volatile> {
  typedef T type;
};

template <typename T>
struct RemovePointer<T* const volatile> {
  typedef const T type;
};

}  // namespace Internals
}  // namespace ArduinoJson
