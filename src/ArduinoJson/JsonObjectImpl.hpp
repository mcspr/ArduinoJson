// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonArray.hpp"
#include "JsonObject.hpp"
#include "JsonObjectSubscript.hpp"

#include "Polyfills/attributes.hpp"
#include "StringTraits/StringTraits.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename TKey>
class JsonMutableObjectSubscript;

template <typename TKey>
class JsonConstObjectSubscript;

// *Always* attached to some JsonObject instance
template <typename TKey>
inline JsonMutableObjectSubscript<TKey>::JsonMutableObjectSubscript() noexcept :
  _object(JsonObject::invalid()),
  _key()
{}

template <typename TKey>
template <typename TKeyRef>
inline JsonMutableObjectSubscript<TKey>::JsonMutableObjectSubscript(JsonObject& object, TKeyRef&& key) :
  _object(object),
  _key(MakeStringRef(std::forward<TKeyRef>(key)))
{}

template <typename TKey>
template <typename TRef>
inline JsonMutableObjectSubscript<TKey>::JsonMutableObjectSubscript(JsonObject& object, StringRefWrapper<TRef> key) :
  _object(object),
  _key(std::move(key))
{}

template <typename TKey>
inline JsonMutableObjectSubscript<TKey>&
JsonMutableObjectSubscript<TKey>::operator=(const JsonMutableObjectSubscript<TKey>& other) {
  return this->operator=(other.as<JsonVariant>());
}

template <typename TKey>
inline JsonMutableObjectSubscript<TKey>&
JsonMutableObjectSubscript<TKey>::operator=(JsonMutableObjectSubscript<TKey>&& other) {
  return this->operator=(other.as<JsonVariant>());
}

template <typename TKey>
inline JsonMutableObjectSubscript<TKey>&
JsonMutableObjectSubscript<TKey>::operator=(const JsonConstObjectSubscript<TKey>& other) {
  return this->operator=(other.template as<JsonVariant>());
}

template <typename TKey>
inline JsonMutableObjectSubscript<TKey>&
JsonMutableObjectSubscript<TKey>::operator=(JsonConstObjectSubscript<TKey>&& other) {
  return this->operator=(other.template as<JsonVariant>());
}

template <typename TKey>
template <typename TValue>
inline JsonMutableObjectSubscript<TKey>&
ARDUINOJSON_FORCE_INLINE JsonMutableObjectSubscript<TKey>::operator=(TValue&& value) {
  set(std::forward<TValue>(value));
  return *this;
}

template <typename TKey>
inline bool JsonMutableObjectSubscript<TKey>::success() const {
  return _object.containsKey(_key.get());
}

template <typename TKey>
template <typename TValue>
inline typename JsonVariantAs<TValue>::type
ARDUINOJSON_FORCE_INLINE JsonMutableObjectSubscript<TKey>::as() const {
  return _object.template get<TValue>(_key.get());
}

template <typename TKey>
template <typename TValue>
inline bool
ARDUINOJSON_FORCE_INLINE JsonMutableObjectSubscript<TKey>::is() const {
  return _object.template is<TValue>(_key.get());
}

template <typename TKey>
template <typename TValue>
inline bool
ARDUINOJSON_FORCE_INLINE JsonMutableObjectSubscript<TKey>::set(TValue&& value) {
  return _object.set(_key.get(), std::forward<TValue>(value));
}

// *Always* attached to some JsonObject instance
template <typename TKey>
inline JsonConstObjectSubscript<TKey>::JsonConstObjectSubscript() noexcept :
  _object(JsonObject::invalid()),
  _key()
{}

template <typename TKey>
template <typename TKeyRef>
inline JsonConstObjectSubscript<TKey>::JsonConstObjectSubscript(const JsonObject& object, TKeyRef&& key) :
  _object(object),
  _key(MakeStringRef(std::forward<TKeyRef>(key)))
{}

template <typename TKey>
template <typename TRef>
inline JsonConstObjectSubscript<TKey>::JsonConstObjectSubscript(const JsonObject& object, StringRefWrapper<TRef> key) :
  _object(object),
  _key(std::move(key))
{}

template <typename TKey>
inline bool JsonConstObjectSubscript<TKey>::success() const {
  return _object.containsKey(_key.get());
}

template <typename TKey>
template <typename TValue>
inline typename JsonVariantAsConst<TValue>::type
ARDUINOJSON_FORCE_INLINE JsonConstObjectSubscript<TKey>::as() const {
  return _object.template get<typename JsonVariantAsConst<TValue>::type>(_key.get());
}

template <typename TKey>
template <typename TValue>
inline bool
ARDUINOJSON_FORCE_INLINE JsonConstObjectSubscript<TKey>::is() const {
  return _object.template is<TValue>(_key.get());
}

#if ARDUINOJSON_ENABLE_STD_STREAM
template <typename TKey>
inline std::ostream&
operator<<(std::ostream& os, const JsonMutableObjectSubscript<TKey>& source) {
  return source.printTo(os);
}

template <typename TKey>
inline std::ostream&
operator<<(std::ostream& os, const JsonConstObjectSubscript<TKey>& source) {
  return source.printTo(os);
}
#endif

}

template <typename TKey>
inline JsonObject::iterator JsonObject::find_impl(TKey key) {
  for (auto it = begin(); it != end(); ++it) {
    const auto ptr = it->key.as<const char*>();
    if (Internals::StringTraits<TKey>::Equals::Operator(key, ptr))
      return it;
  }
  return end();
}

template <typename TValue, typename TKey>
inline TValue JsonObject::get_impl(TKey key) {
    iterator it = find_impl(std::move(key));
    return it != end() ? it->value.as<TValue>()
                       : Internals::JsonVariantDefault<TValue>::get();
}

template <typename TKey, typename TValue>
inline bool JsonObject::set_impl(TKey key, TValue value) {
  // when creating a key, prune failed list entry before returning
  bool out = false;

  // search for existing or add another kv object
  auto it = find_impl(Internals::MakeStringRef(key.get()));
  if (it == end()) {
    it = add();
    if (it == end())
      return false;

    if (!Internals::ValueSaver<TKey>::save(
      _buffer, it->key, std::move(key)))
    {
      remove(it);
      return false;
    }

    out = true;
  }

  if (it != end()) {
    if (!Internals::ValueSaver<TValue>::save(
      _buffer, it->value, std::move(value)))
    {
      if (out)
        remove(it);

      return false;
    }

    out = true;
  }

  return out;
}

template <typename TValue, typename TKey>
bool JsonObject::is_impl(TKey key) const {
    const_iterator it = find_impl(std::move(key));
    return it != end() ? it->value.is<TValue>() : false;
}

template <typename TKey>
inline JsonArray& JsonObject::createNestedArray_impl(TKey key) {
  auto &ref = _buffer->createArray();
  if (set_impl(std::move(key), JsonVariant(ref)))
    return ref;
  return JsonArray::invalid();
}

template <typename TKey>
inline JsonObject& JsonObject::createNestedObject_impl(TKey key) {
  JsonObject &ref = _buffer->createObject();
  if (set_impl(std::move(key), JsonVariant(ref)))
    return ref;
  return JsonObject::invalid();
}

template <typename TKey>
typename Internals::JsonObjectSubscriptHelper<const JsonObject, TKey>::subscript_type
inline JsonObject::operator[](TKey&& key) const {
  return typename Internals::JsonObjectSubscriptHelper<const JsonObject, TKey>::subscript_type(
    *this, std::forward<TKey>(key));
}

template <typename TKey>
typename Internals::JsonObjectSubscriptHelper<JsonObject, TKey>::subscript_type
inline JsonObject::operator[](TKey&& key) {
  return typename Internals::JsonObjectSubscriptHelper<JsonObject, TKey>::subscript_type(
    *this, std::forward<TKey>(key));
}

}  // namespace ArduinoJson
