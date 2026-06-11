// MIT License

#include "RemoveConstReference.hpp"

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

template <typename T>
struct WithRvalueReference {
  typedef T&& type;
};

template <typename T>
struct WithRvalueReference<T&> {
  typedef T&& type;
};

template <typename T>
struct WithRvalueReference<T&&> {
  typedef T&& type;
};

template <typename T>
using DeclvalReturnType = typename WithRvalueReference<
  typename RemoveConstReference<T>::type>::type;

}

template <typename T>
typename TypeTraits::DeclvalReturnType<T> Declval() noexcept;

}
}
