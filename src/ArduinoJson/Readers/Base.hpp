// MIT License

#pragma once

#include "../TypeTraits/RemoveConstReference.hpp"

namespace ArduinoJson {
namespace Internals {
namespace Readers {

template <typename T, typename = void>
struct ReaderImplBase {
};

template <typename T>
struct ReaderImplHelper {
  typedef T raw_type;
  typedef typename RemoveConstReference<T>::type type;
  typedef ReaderImplBase<type> impl_type;
};

}

template <typename T>
struct ReaderImpl : Readers::ReaderImplHelper<T>::impl_type {
  using Readers::ReaderImplHelper<T>::impl_type::impl_type;
};

}
}
