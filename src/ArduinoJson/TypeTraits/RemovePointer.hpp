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
struct RemovePointer<const T*> {
  typedef T type;
};

template <typename T>
struct RemovePointer<volatile T*> {
  typedef T type;
};

template <typename T>
struct RemovePointer<const volatile T*> {
  typedef T type;
};

}  // namespace Internals
}  // namespace ArduinoJson
