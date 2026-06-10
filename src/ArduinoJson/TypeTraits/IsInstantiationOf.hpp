// MIT License

#pragma once

#include "Constant.hpp"

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

template <typename T, template <typename...> class Template>
struct IsInstantiationOfImpl : FalseType {
};

template <template <typename...> class Template, typename... Args>
struct IsInstantiationOfImpl<Template<Args...>, Template> : TrueType {
};

}

template <template <typename...> class Template, typename T>
struct IsInstantiationOf : TypeTraits::IsInstantiationOfImpl<T, Template> {
};

}
}
