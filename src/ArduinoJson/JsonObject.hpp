// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "StringTraits/StringTraits.hpp"

#include "Data/JsonBufferAllocated.hpp"
#include "Data/JsonVariantDefault.hpp"
#include "Data/List.hpp"
#include "Data/ReferenceType.hpp"
#include "Data/ValueSaver.hpp"
#include "Data/StringView.hpp"

#include "Serialization/JsonPrintable.hpp"

#include "TypeTraits/EnableIf.hpp"

#include "JsonPair.hpp"

// Returns the size (in bytes) of an object with n elements.
// Can be very handy to determine the size of a StaticJsonBuffer.
#define JSON_OBJECT_SIZE(NUMBER_OF_ELEMENTS) \
  (sizeof(JsonObject) + (NUMBER_OF_ELEMENTS) * sizeof(JsonObject::node_type))

namespace ArduinoJson {

// Forward declarations

class JsonArray;
class JsonBuffer;

namespace Internals {

template <typename, typename>
struct JsonObjectSubscriptHelper;

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
  explicit JsonObject(JsonBuffer* buffer) noexcept :
    Internals::List<JsonPair>(buffer)
  {}

  // Gets the value associated with the specified key.
  template <typename TKey>
  typename Internals::JsonObjectSubscriptHelper<const JsonObject, TKey>::subscript_type
  operator[](TKey&&) const;

  // Gets or sets the value associated with the specified key.
  template <typename TKey>
  typename Internals::JsonObjectSubscriptHelper<JsonObject, TKey>::subscript_type
  operator[](TKey&&);

  // Sets the specified key with the specified value.
  // Both TKey and TValue allow for arbitrary string types.
  template <typename TValue, typename TKey>
  typename Internals::EnableIf<
    Internals::Or<Internals::IsVariant<TValue>,
                  Internals::IsIntegral<TValue>,
                  Internals::IsFloatingPoint<TValue>,
                  Internals::IsSame<JsonNull, TValue>>::value,
    bool>::type
  ARDUINOJSON_FORCE_INLINE set(TKey&& key, TValue value) {
    return set_impl(Internals::MakeStringRef(std::forward<TKey>(key)), value);
  }

  template <typename TKey, typename TValue>
  typename Internals::EnableIf<
    Internals::IsSame<std::nullptr_t, TValue>::value, bool>::type
  set(TKey&&, TValue) {
    static_assert(!Internals::IsSame<TValue, std::nullptr_t>::value,
      "ambiguous set(..., nullptr)");
  }

  template <typename TKey, typename TValue>
  typename Internals::EnableIf<
    Internals::IsReferenceType<TValue>::value, bool>::type
  ARDUINOJSON_FORCE_INLINE set(TKey&& key, TValue& ref) {
    return set_impl(
      Internals::MakeStringRef(std::forward<TKey>(key)),
      JsonVariant(ref));
  }

  template <typename TValue, typename TKey>
  typename Internals::EnableIf<Internals::HasStringTraits<TValue>::value, bool>::type
  ARDUINOJSON_FORCE_INLINE set(TKey&& key, TValue&& value) {
    return set_impl(
      Internals::MakeStringRef(std::forward<TKey>(key)),
      Internals::MakeStringRef(std::forward<TValue>(value)));
  }

  // Gets the value associated with the specified key.
  template <typename TValue, typename TKey>
  typename Internals::JsonVariantAs<TValue>::type
  ARDUINOJSON_FORCE_INLINE get(TKey&& key) {
    return get_impl<typename Internals::JsonVariantAs<TValue>::type>(
      Internals::MakeStringRef(std::forward<TKey>(key)));
  }

  template <typename TValue, typename TKey>
  typename Internals::JsonVariantAsConst<TValue>::type
  ARDUINOJSON_FORCE_INLINE get(TKey&& key) const {
    return const_cast<JsonObject *>(this)->
      get<typename Internals::JsonVariantAsConst<TValue>::type>(std::forward<TKey>(key));
  }

  template <typename TValue, typename TKey,
    typename Internals::EnableIf<
      Internals::IsSame<std::nullptr_t, TValue>::value>::type>
  typename Internals::JsonVariantAs<TValue>::type
  get(TKey&&) const {
    static_assert(!Internals::IsSame<TValue, std::nullptr_t>::value,
      "ambiguous get<std::nullptr_t>(...)");
  }

  template <typename TValue>
  typename Internals::JsonVariantAs<TValue>::type
  get(std::nullptr_t) const {
    static_assert(!Internals::IsSame<TValue, std::nullptr_t>::value,
      "ambiguous get(nullptr)");
  }

  // Checks the type of the value associated with the specified key.
  template <typename TValue, typename TKey>
  bool ARDUINOJSON_FORCE_INLINE is(TKey&& key) const {
    return is_impl<typename Internals::JsonVariantAs<TValue>::type>(
            Internals::MakeStringRef(std::forward<TKey>(key)));
  }

  template <typename TValue, typename TKey,
    typename Internals::EnableIf<
      Internals::IsSame<std::nullptr_t, TValue>::value>::type>
  typename Internals::JsonVariantAs<TValue>::type
  is(TKey&&) const {
    static_assert(!Internals::IsSame<TValue, std::nullptr_t>::value,
      "ambiguous is<std::nullptr_t>(...)");
  }

  // Creates and adds an object entry containing JsonArray.
  template <typename TKey>
  JsonArray& ARDUINOJSON_FORCE_INLINE createNestedArray(TKey&& key) {
    return createNestedArray_impl(
      Internals::MakeStringRef(std::forward<TKey>(key)));
  }

  // Creates and adds an object entry containing JsonObject.
  template <typename TKey>
  JsonObject& ARDUINOJSON_FORCE_INLINE createNestedObject(TKey&& key) {
    return createNestedObject_impl(
      Internals::MakeStringRef(std::forward<TKey>(key)));
  }

  // Tells weither the specified key is present and associated with a value.
  template <typename TKey>
  bool ARDUINOJSON_FORCE_INLINE containsKey(TKey&& key) const {
    return contains_impl(Internals::MakeStringRef(std::forward<TKey>(key)));
  }

  // Removes the specified key and the associated value.
  template <typename TKey>
  void ARDUINOJSON_FORCE_INLINE remove(TKey&& key) {
    remove(find_impl(Internals::MakeStringRef(std::forward<TKey>(key))));
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
  iterator find_impl(TKey key);

  template <typename TKey>
  const_iterator find_impl(TKey key) const {
    return const_cast<JsonObject *>(this)->find_impl(std::move(key));
  }

  template <typename TValue, typename TKey>
  TValue get_impl(TKey key);

  template <typename TKey, typename TValue>
  bool set_impl(TKey key, TValue value);

  template <typename TValue, typename TKey>
  bool is_impl(TKey key) const;

  template <typename TKey>
  bool contains_impl(TKey key) const {
    return find_impl(std::move(key)) != end();
  }

  template <typename TKey>
  JsonArray& createNestedArray_impl(TKey);

  template <typename TKey>
  JsonObject& createNestedObject_impl(TKey);

  template <typename>
  friend class Internals::JsonMutableObjectSubscript;

  template <typename>
  friend class Internals::JsonConstObjectSubscript;

  template <typename, typename>
  friend class Internals::JsonParser;
};

namespace Internals {

template <>
struct JsonVariantDefault<JsonObject> {
  static JsonObject& get() {
    return JsonObject::invalid();
  }
};

template <>
struct JsonVariantDefault<const JsonObject> {
  static const JsonObject& get() {
    return JsonObject::invalid();
  }
};

template <>
struct JsonVariantDefault<JsonObject&> {
  static JsonObject& get() {
    return JsonObject::invalid();
  }
};

template <>
struct JsonVariantDefault<const JsonObject&> {
  static const JsonObject& get() {
    return JsonObject::invalid();
  }
};

}  // namespace Internals
}  // namespace ArduinoJson
