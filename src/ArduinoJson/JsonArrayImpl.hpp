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
inline JsonMutableArraySubscript::JsonMutableArraySubscript() noexcept :
  _array(JsonArray::invalid())
{}

inline JsonMutableArraySubscript::JsonMutableArraySubscript(JsonArray& array, size_t index) noexcept :
  _array(array),
  _index(index)
{}

inline JsonMutableArraySubscript&
JsonMutableArraySubscript::operator=(const JsonMutableArraySubscript& other) {
  return this->operator=(other.as<JsonVariant>());
}

inline JsonMutableArraySubscript&
JsonMutableArraySubscript::operator=(JsonMutableArraySubscript&& other) {
  return this->operator=(other.as<JsonVariant>());
}

inline JsonMutableArraySubscript&
JsonMutableArraySubscript::operator=(const JsonConstArraySubscript& other) {
  return this->operator=(other.as<JsonVariant>());
}

inline JsonMutableArraySubscript&
JsonMutableArraySubscript::operator=(JsonConstArraySubscript&& other) {
  return this->operator=(other.as<JsonVariant>());
}

template <typename TValue>
inline JsonMutableArraySubscript&
ARDUINOJSON_FORCE_INLINE JsonMutableArraySubscript::operator=(TValue&& value) {
  set(std::forward<TValue>(value));
  return *this;
}

inline bool JsonMutableArraySubscript::success() const {
  return _array.success() && _index < _array.size();
}

template <typename TValue>
inline typename JsonVariantAs<TValue>::type
ARDUINOJSON_FORCE_INLINE JsonMutableArraySubscript::as() const {
  return _array.get<TValue>(_index);
}

template <typename TValue>
inline bool
ARDUINOJSON_FORCE_INLINE JsonMutableArraySubscript::is() const {
  return _array.is<TValue>(_index);
}

template <typename TValue>
inline bool
ARDUINOJSON_FORCE_INLINE JsonMutableArraySubscript::set(TValue&& value) {
  return _array.set(_index, std::forward<TValue>(value));
}

inline JsonConstArraySubscript::JsonConstArraySubscript() noexcept :
  _array(JsonArray::invalid())
{}

inline JsonConstArraySubscript::JsonConstArraySubscript(const JsonArray& array, size_t index) noexcept :
  _array(array),
  _index(index)
{}

inline JsonConstArraySubscript::JsonConstArraySubscript(const JsonConstArraySubscript& other) noexcept :
  _array(other._array),
  _index(other._index)
{}

inline JsonConstArraySubscript::JsonConstArraySubscript(JsonConstArraySubscript&& other) noexcept :
  _array(other._array),
  _index(other._index)
{}

inline JsonConstArraySubscript::JsonConstArraySubscript(const JsonMutableArraySubscript& other) noexcept :
  _array(other._array),
  _index(other._index)
{}

inline JsonConstArraySubscript::JsonConstArraySubscript(JsonMutableArraySubscript&& other) noexcept :
  _array(other._array),
  _index(other._index)
{}

inline bool JsonConstArraySubscript::success() const {
  return _array.success() && _index < _array.size();
}

template <typename TValue>
inline typename JsonVariantAsConst<TValue>::type
ARDUINOJSON_FORCE_INLINE JsonConstArraySubscript::as() const {
  return _array.get<typename JsonVariantAsConst<TValue>::type>(_index);
}

template <typename TValue>
inline bool
ARDUINOJSON_FORCE_INLINE JsonConstArraySubscript::is() const {
  return _array.is<TValue>(_index);
}

#if ARDUINOJSON_ENABLE_STD_STREAM
inline std::ostream&
operator<<(std::ostream& os, const JsonMutableArraySubscript& source) {
  return source.printTo(os);
}

inline std::ostream&
operator<<(std::ostream& os, const JsonConstArraySubscript& source) {
  return source.printTo(os);
}
#endif

template <typename TImpl>
JsonConstArraySubscript
inline JsonConstSubscripts<TImpl>::operator[](size_t index) const {
  return JsonConstArraySubscript(impl()->template as<const JsonArray&>(), index);
}

template <typename TImpl>
JsonMutableArraySubscript
inline JsonMutableSubscripts<TImpl>::operator[](size_t index) const {
  return JsonMutableArraySubscript(impl()->template as<JsonArray&>(), index);
}

}  // namespace Internals

inline Internals::JsonConstArraySubscript JsonArray::operator[](
    size_t index) const {
  return Internals::JsonConstArraySubscript(*this, index);
}

inline Internals::JsonMutableArraySubscript JsonArray::operator[](size_t index) {
  return Internals::JsonMutableArraySubscript(*this, index);
}

inline JsonArray& JsonArray::createNestedArray() {
  JsonArray &ref = _buffer->createArray();
  if (add_impl(JsonVariant(ref)))
    return ref;
  return JsonArray::invalid();
}

inline JsonObject& JsonArray::createNestedObject() {
  JsonObject &ref = _buffer->createObject();
  if (add_impl(JsonVariant(ref)))
    return ref;
  return JsonObject::invalid();
}

}  // namespace ArduinoJson
