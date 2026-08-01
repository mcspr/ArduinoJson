// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonArray.hpp"
#include "JsonObject.hpp"
#include "JsonArraySubscript.hpp"

namespace ArduinoJson {
namespace Internals {

// *Always* attached to some JsonArray instance
inline JsonArraySubscript::JsonArraySubscript() noexcept :
  _array(JsonArray::invalid())
{}

inline JsonArraySubscript::JsonArraySubscript(JsonArray& array, size_t index) noexcept :
  _array(array),
  _index(index)
{}

inline JsonArraySubscript&
ARDUINOJSON_FORCE_INLINE JsonArraySubscript::operator=(const JsonArraySubscript& src) {
  _array.set_impl(_index, src.template as<JsonVariant>());
  return *this;
}

template <typename TValue>
inline JsonArraySubscript&
ARDUINOJSON_FORCE_INLINE JsonArraySubscript::operator=(TValue&& value) {
  _array.set(_index, std::forward<TValue>(value));
  return *this;
}

inline bool JsonArraySubscript::success() const {
  return _array.success() && _index < _array.size();
}

template <typename TValue>
inline typename JsonVariantAs<TValue>::type
ARDUINOJSON_FORCE_INLINE JsonArraySubscript::as() {
  return _array.get<TValue>(_index);
}

template <typename TValue>
inline bool
ARDUINOJSON_FORCE_INLINE JsonArraySubscript::is() const {
  return _array.is<TValue>(_index);
}

template <typename TValue>
inline bool
ARDUINOJSON_FORCE_INLINE JsonArraySubscript::set(TValue&& value) {
  return _array.set(_index, std::forward<TValue>(value));
}

template <typename TImpl>
inline JsonArraySubscript JsonVariantSubscripts<TImpl>::operator[](size_t index) {
  return impl()->template as<JsonArray &>()[index];
}

template <typename TImpl>
inline JsonArraySubscript
ARDUINOJSON_FORCE_INLINE JsonVariantSubscripts<TImpl>::operator[](size_t index) const {
  return impl()->template as<const JsonArray &>()[index];
}

}  // namespace Internals

inline Internals::JsonArraySubscript JsonArray::operator[](size_t index) {
  return Internals::JsonArraySubscript(*this, index);
}

inline const Internals::JsonArraySubscript JsonArray::operator[](
    size_t index) const {
  return Internals::JsonArraySubscript(*const_cast<JsonArray*>(this), index);
}

inline JsonArray& JsonArray::createNestedArray() {
  JsonArray &ref = _buffer->createArray();
  add(JsonVariant(ref));
  return ref;
}

inline JsonObject& JsonArray::createNestedObject() {
  JsonObject &ref = _buffer->createObject();
  add(JsonVariant(ref));
  return ref;
}

}  // namespace ArduinoJson
