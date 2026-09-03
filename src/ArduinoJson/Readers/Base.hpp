// MIT License

#pragma once

#include "../TypeTraits/RemoveConstReference.hpp"

namespace ArduinoJson {
namespace Internals {
namespace Readers {

template <typename TJson, typename = void>
struct ReaderImplBase {
};

template <typename TJson>
struct ReaderImplHelper {
  using raw_input_type = TJson;
  using input_type = typename RemoveConstReference<raw_input_type>::type;
  using type = typename ReaderImplBase<input_type>::type;
};

}

template <typename T>
struct ReaderImpl {
  using type = typename Readers::ReaderImplHelper<T>::type;
};

}
}
