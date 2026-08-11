// MIT License

#pragma once

#include "Constant.hpp"

namespace ArduinoJson {

class JsonArray;
class JsonObject;

namespace Internals {
namespace TypeTraits {

template <typename>
struct IsJsonReferenceType : FalseType {
};

template <>
struct IsJsonReferenceType<JsonArray> : TrueType {
};

template <>
struct IsJsonReferenceType<JsonObject> : TrueType {
};

template <typename>
struct IsJsonConstReferenceType : FalseType {
};

template <>
struct IsJsonConstReferenceType<const JsonArray> : TrueType {
};

template <>
struct IsJsonConstReferenceType<const JsonObject> : TrueType {
};

}

template <typename T>
struct IsJsonReference : TypeTraits::IsJsonReferenceType<T>::type {
};

template <typename T>
struct IsJsonReference<T const> : TypeTraits::IsJsonReferenceType<T>::type {
};

template <typename T>
struct IsJsonConstReference : TypeTraits::IsJsonConstReferenceType<T>::type {
};

}
}
