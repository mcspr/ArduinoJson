// MIT License

#pragma once

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

template <typename ...>
struct MakeVoidType {
  typedef void type;
};

}

// aka std::void_t from -std=c++17
template <typename ...T>
using VoidType = typename TypeTraits::MakeVoidType<T...>::type;

}
}
