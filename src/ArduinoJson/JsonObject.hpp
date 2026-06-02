// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Data/JsonBufferAllocated.hpp"
#include "Data/List.hpp"
#include "Data/ReferenceType.hpp"
#include "Data/ValueSaver.hpp"
#include "JsonPair.hpp"
#include "Serialization/JsonPrintable.hpp"
#include "StringTraits/StringTraits.hpp"
#include "TypeTraits/EnableIf.hpp"
#include "TypeTraits/IsArray.hpp"
#include "TypeTraits/IsFloatingPoint.hpp"
#include "TypeTraits/IsSame.hpp"

// Returns the size (in bytes) of an object with n elements.
// Can be very handy to determine the size of a StaticJsonBuffer.
#define JSON_OBJECT_SIZE(NUMBER_OF_ELEMENTS) \
  (sizeof(JsonObject) + (NUMBER_OF_ELEMENTS) * sizeof(JsonObject::node_type))

namespace ArduinoJson {

// Forward declarations
class JsonArray;
class JsonBuffer;
namespace Internals {
template <typename>
class JsonObjectSubscript;
}

// A dictionary of JsonVariant indexed by string (char*)
//
// The constructor is private, instances must be created via
// JsonBuffer::createObject() or JsonBuffer::parseObject().
// A JsonObject can be serialized to a JSON string via JsonObject::printTo().
// It can also be deserialized from a JSON string via JsonBuffer::parseObject().
class JsonObject : public Internals::JsonPrintable<JsonObject>,
                   public Internals::ReferenceType,
                   public Internals::NonCopyable,
                   public Internals::List<JsonPair>,
                   public Internals::JsonBufferAllocated {
 public:
  // Create an empty JsonArray attached to the specified JsonBuffer.
  // You should not use this constructor directly.
  // Instead, use JsonBuffer::createObject() or JsonBuffer.parseObject().
  explicit JsonObject(JsonBuffer* buffer) throw()
      : Internals::List<JsonPair>(buffer) {}

  // Gets or sets the value associated with the specified key.
  //
  // JsonObjectSubscript operator[](TKey)
  // TKey = char*, const char*, char[], const char[N], const FlashStringHelper*
  // TKey = const std::string&, const String&
  template <typename TKey>
  Internals::JsonObjectSubscript<TKey> operator[](
      TKey&& key) {
    return Internals::JsonObjectSubscript<TKey>(*this, std::forward<TKey>(key));
  }

  template <typename TChar, size_t Size>
  inline Internals::JsonObjectSubscript<TChar*> operator[](
      TChar (&)[Size]);

  // Gets the value associated with the specified key.
  //
  // const JsonObjectSubscript operator[](TKey) const;
  // TKey = const std::string&, const String&
  template <typename TKey>
  const Internals::JsonObjectSubscript<TKey> operator[](
      TKey&& key) const {
    return Internals::JsonObjectSubscript<TKey>(
        *const_cast<JsonObject*>(this), std::forward<TKey>(key));
  }

  //
  // const JsonObjectSubscript operator[](TKey) const;
  // TKey = const char*, const char[N], const FlashStringHelper*
  template <typename TChar, size_t Size>
  const Internals::JsonObjectSubscript<TChar*> operator[](
      TChar (&key)[Size]) const;

  // Sets the specified key with the specified value.
  //
  // bool set(TKey, TValue);
  // TKey = const std::string&, const String&
  // TValue = bool, long, int, short, float, double, RawJson, JsonVariant,
  //          std::string, String, JsonArray, JsonObject
  template <typename TValue, typename TKey>
  bool set(TKey&& key, TValue&& value) {
    return set_impl(std::forward<TKey>(key), std::forward<TValue>(value));
  }

  template <typename TValue, typename TChar, size_t Size,
    typename Internals::EnableIf<Internals::IsChar<TChar>::value>::type* = nullptr>
  bool set(TChar (&key)[Size], TValue&& value) {
    return set_impl(&key[0], std::forward<TValue>(value));
  }

  template <typename TValue, size_t ValueSize, typename TKey, size_t KeySize,
    typename Internals::EnableIf<Internals::IsChar<TValue>::value>::type* = nullptr,
    typename Internals::EnableIf<Internals::IsChar<TKey>::value>::type* = nullptr>
  bool set(TKey (&key)[KeySize], TValue (&value)[ValueSize]) {
    return set_impl(key, value);
  }

  template <typename TValue, size_t ValueSize, typename TKey,
    typename Internals::EnableIf<Internals::IsChar<TValue>::value>::type* = nullptr>
  bool set(TKey &&key, TValue (&value)[ValueSize]) {
    return set_impl(std::forward<TKey>(key), value);
  }

  // Gets the value associated with the specified key.
  //
  // TValue get<TValue>(TKey) const;
  // TKey = const std::string&, const String&
  // TValue = bool, char, long, int, short, float, double,
  //          std::string, String, JsonArray, JsonObject
  template <typename TValue, typename TKey>
  typename Internals::JsonVariantAs<TValue>::type get(
      TKey&& key) const {
    return get_impl<typename Internals::JsonVariantAs<TValue>::type>(
      std::forward<TKey>(key));
  }

  template <typename TValue>
  typename Internals::JsonVariantAs<TValue>::type get(
      std::nullptr_t) const = delete;

  // Checks the type of the value associated with the specified key.
  //
  //
  // bool is<TValue>(TKey) const;
  // TKey = const std::string&, const String&
  // TValue = bool, char, long, int, short, float, double,
  //          std::string, String, JsonArray, JsonObject
  template <typename TValue, typename TKey>
  bool is(TKey&& key) const {
    return is_impl<typename Internals::JsonVariantAs<TValue>::type>(
            std::forward<TKey>(key));
  }
  //
  // bool is<TValue>(TKey) const;
  // TKey = char*, const char*, const FlashStringHelper*
  // TValue = bool, char, long, int, short, float, double,
  //          std::string, String, JsonArray, JsonObject
  template <typename TValue, typename TChar, size_t Size>
  bool is(TChar (&key)[Size]) const {
    return is_impl<typename Internals::JsonVariantAs<TValue>::type>(&key[0]);
  }

  // Creates and adds a JsonArray.
  //
  // JsonArray& createNestedArray(TKey);
  // TKey = const std::string&, const String&
  template <typename TKey>
  JsonArray& createNestedArray(TKey&& key) {
    return createNestedArray_impl(std::forward<TKey>(key));
  }
  // JsonArray& createNestedArray(TKey);
  // TKey = char*, const char*, char[], const char[], const FlashStringHelper*
  template <typename TChar, size_t Size>
  JsonArray& createNestedArray(TChar (&key)[Size]) {
    return createNestedArray_impl(&key[0]);
  }

  // Creates and adds a JsonObject.
  //
  // JsonObject& createNestedObject(TKey);
  // TKey = const std::string&, const String&
  template <typename TKey>
  JsonObject& createNestedObject(TKey&& key) {
    return createNestedObject_impl(std::forward<TKey>(key));
  }
  //
  // JsonObject& createNestedObject(TKey);
  // TKey = char*, const char*, char[], const char[], const FlashStringHelper*
  template <typename TChar, size_t Size>
  JsonObject& createNestedObject(TChar (&key)[Size]) {
    return createNestedObject_impl(&key[0]);
  }

  // Tells weither the specified key is present and associated with a value.
  //
  // bool containsKey(TKey);
  // TKey = const std::string&, const String&
  template <typename TKey>
  bool containsKey(TKey&& key) const {
    return findKey(std::forward<TKey>(key)) != end();
  }
  //
  // bool containsKey(TKey);
  // TKey = char*, const char*, char[], const char[], const FlashStringHelper*
  template <typename TChar, size_t Size>
  bool containsKey(TChar (&key)[Size]) const {
    return findKey(&key[0]) != end();
  }

  // Removes the specified key and the associated value.
  //
  // void remove(TKey);
  // TKey = const std::string&, const String&
  template <typename TKey>
  void remove(TKey&& key) {
    remove(findKey(std::forward<TKey>(key)));
  }
  //
  // void remove(TKey);
  // TKey = char*, const char*, char[], const char[], const FlashStringHelper*
  template <typename TChar, size_t Size>
  void remove(TChar (&key)[Size]) {
    remove(findKey(&key[0]));
  }
  //
  // void remove(iterator)
  using Internals::List<JsonPair>::remove;

  // Returns a reference an invalid JsonObject.
  // This object is meant to replace a `_buffer(nullptr)`
  // when memory allocation or JSON parsing fail.
  static JsonObject& invalid() {
    static JsonObject instance(Internals::EmptyJsonBuffer::instance());
    return instance;
  }

 private:
  // Returns the list node that matches the specified key.
  template <typename TKey>
  iterator findKey(TKey&& key) {
    iterator it;
    for (it = begin(); it != end(); ++it) {
      if (Internals::StringTraits<TKey>::equals(key, it->key)) break;
    }
    return it;
  }

  template <typename TKey>
  const_iterator findKey(TKey&& key) const {
    return const_cast<JsonObject*>(this)->findKey(std::forward<TKey>(key));
  }

  template <typename TValue, typename TKey>
  typename Internals::JsonVariantAs<TValue>::type get_impl(
      TKey&& key) const {
    const_iterator it = findKey(std::forward<TKey>(key));
    return it != end() ? it->value.as<TValue>()
                       : Internals::JsonVariantDefault<TValue>::get();
  }

  template <typename TKey, typename TValue>
  bool set_impl(TKey&& key, TValue&& value) {
    if (Internals::StringTraits<TKey>::is_null(key))
      return false;

    // search a matching key
    iterator it = findKey(key);
    if (it == end()) {
      // or add the key
      it = add();
      if (it == end())
        return false;
      const auto key_saved = Internals::ValueSaver<TKey>::save(
        _buffer, it->key, std::forward<TKey>(key));
      if (!key_saved)
        return false;
    }

    if (it != end())
      return Internals::ValueSaver<TValue>::save(
        _buffer, it->value, std::forward<TValue>(value));

    return false;
  }

  template <typename TValue, typename TKey>
  bool is_impl(TKey&& key) const {
    const_iterator it = findKey(std::forward<TKey>(key));
    return it != end() ? it->value.is<TValue>() : false;
  }

  template <typename TKey>
  JsonArray& createNestedArray_impl(TKey&& key);

  template <typename TKey>
  JsonObject& createNestedObject_impl(TKey&& key);
};

namespace Internals {
template <>
struct JsonVariantDefault<JsonObject> {
  static JsonObject& get() {
    return JsonObject::invalid();
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
