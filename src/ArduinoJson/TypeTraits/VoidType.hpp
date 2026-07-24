// MIT License

#pragma once

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=64395
// workaround for gcc4.8 & gcc4.9 broken void_t as template param
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
