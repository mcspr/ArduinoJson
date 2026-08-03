// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "ArduinoJson/Polyfills/attributes.hpp"
#include "ArduinoJson/StringTraits/StringTraitsBase.hpp"
#include "ArduinoJson/TypeTraits/IsVariant.hpp"
#include "Data/JsonBufferAllocated.hpp"
#include "Data/JsonVariantDefault.hpp"
#include "Data/List.hpp"
#include "Data/ReferenceType.hpp"
#include "Data/ValueSaver.hpp"
#include "Data/StringView.hpp"
#include "JsonVariant.hpp"
#include "Serialization/JsonPrintable.hpp"
#include "StringTraits/StringTraits.hpp"
#include "TypeTraits/EnableIf.hpp"
#include "TypeTraits/IsIntegral.hpp"
#include "TypeTraits/IsFloatingPoint.hpp"
#include "TypeTraits/IsSame.hpp"
#include "TypeTraits/Or.hpp"

// Returns the size (in bytes) of an array with n elements.
// Can be very handy to determine the size of a StaticJsonBuffer.
#define JSON_ARRAY_SIZE(NUMBER_OF_ELEMENTS) \
  (sizeof(JsonArray) + (NUMBER_OF_ELEMENTS) * sizeof(JsonArray::node_type))

namespace ArduinoJson {

// Forward declarations
class JsonObject;
class JsonBuffer;

namespace Internals {

class JsonMutableArraySubscript;
class JsonConstArraySubscript;

}

// An array of JsonVariant.
//
// The constructor is private, instances must be created via
// JsonBuffer::createArray() or JsonBuffer::parseArray().
// A JsonArray can be serialized to a JSON string via JsonArray::printTo().
// It can also be deserialized from a JSON string via JsonBuffer::parseArray().
class JsonArray : public Internals::JsonPrintable<JsonArray>,
                  public Internals::ReferenceType,
                  public Internals::NonCopyable,
                  public Internals::List<JsonVariant>,
                  public Internals::JsonBufferAllocated {
 public:
  // Create an empty JsonArray attached to the specified JsonBuffer.
  // You should not call this constructor directly.
  // Instead, use JsonBuffer::createArray() or JsonBuffer::parseArray().
  explicit JsonArray(JsonBuffer* buffer) noexcept :
    Internals::List<JsonVariant>(buffer)
  {}

  // Gets the value at the specified index
  Internals::JsonConstArraySubscript operator[](size_t index) const;

  // Gets or sets the value at specified index
  Internals::JsonMutableArraySubscript operator[](size_t index);

  // Adds the specified value at the end of the array.
  template <typename TValue>
  typename Internals::EnableIf<
      Internals::Or<Internals::IsIntegral<TValue>,
                    Internals::IsFloatingPoint<TValue>,
                    Internals::IsSame<JsonNull, TValue>>::value,
    bool>::type
  ARDUINOJSON_FORCE_INLINE add(TValue value) {
    return add_impl(value);
  }

  template <typename TValue>
  typename Internals::EnableIf<
    Internals::IsSame<std::nullptr_t, TValue>::value, bool>::type
  add(TValue) {
    static_assert(!Internals::IsSame<TValue, std::nullptr_t>::value,
      "ambiguous add(nullptr)");
  }

  template <typename TValue>
  typename Internals::EnableIf<
    Internals::IsVariant<TValue>::value, bool>::type
  ARDUINOJSON_FORCE_INLINE add(TValue&& value) {
    return add_impl(value.template as<JsonVariant>());
  }

  template <typename TValue>
  typename Internals::EnableIf<
    Internals::IsReferenceType<TValue>::value, bool>::type
  ARDUINOJSON_FORCE_INLINE add(TValue& ref) {
    return add_impl(JsonVariant(ref));
  }

  template <typename TValue>
  typename Internals::EnableIf<
    Internals::HasStringTraits<TValue>::value, bool>::type
  ARDUINOJSON_FORCE_INLINE add(TValue&& value) {
    return add_impl(Internals::MakeStringRef(std::forward<TValue>(value)));
  }

  // Sets the value at specified index.
  template <typename TValue>
  typename Internals::EnableIf<
      Internals::Or<Internals::IsIntegral<TValue>,
                    Internals::IsFloatingPoint<TValue>,
                    Internals::IsSame<JsonNull, TValue>>::value,
      bool>::type
  ARDUINOJSON_FORCE_INLINE set(size_t index, TValue value) {
    return set_impl(index, value);
  }

  template <typename TValue>
  typename Internals::EnableIf<
      Internals::IsVariant<TValue>::value, bool>::type
  ARDUINOJSON_FORCE_INLINE set(size_t index, TValue&& value) {
    return set_impl(index, value.template as<JsonVariant>());
  }

  template <typename TValue>
  typename Internals::EnableIf<
    Internals::IsSame<std::nullptr_t, TValue>::value, bool>::type
  set(size_t, TValue) {
    static_assert(!Internals::IsSame<TValue, std::nullptr_t>::value,
      "ambiguous set(..., nullptr)");
  }

  template <typename TValue>
  typename Internals::EnableIf<Internals::HasStringTraits<TValue>::value, bool>::type
  ARDUINOJSON_FORCE_INLINE set(size_t index, TValue&& value) {
    return set_impl(index, Internals::MakeStringRef(std::forward<TValue>(value)));
  }

  template <typename TValue>
  typename Internals::EnableIf<
    Internals::IsReferenceType<TValue>::value, bool>::type
  ARDUINOJSON_FORCE_INLINE set(size_t index, TValue& ref) {
    return set_impl(index, JsonVariant(ref));
  }

  template <typename T>
  typename Internals::EnableIf<Internals::IsFloatingPoint<T>::value, bool>::type
  ARDUINOJSON_FORCE_INLINE set(size_t index, T value, uint8_t decimals) {
    return set_impl(index, JsonVariant(value, decimals));
  }

  // Gets the value at the specified index.
  template <typename TValue>
  typename Internals::JsonVariantAs<TValue>::type
  get(size_t index) {
    const auto it = begin() + index;
    return it != end() ? it->as<TValue>() : Internals::JsonVariantDefault<TValue>::get();
  }

  template <typename TValue>
  typename Internals::JsonVariantAsConst<TValue>::type
  get(size_t index) const {
    return const_cast<JsonArray *>(this)->
      get<typename Internals::JsonVariantAsConst<TValue>::type>(index);
  }

  // Check the type of the value at specified index.
  template <typename T>
  bool is(size_t index) const {
    const auto it = begin() + index;
    return it != end() ? it->is<T>() : false;
  }

  // Creates a JsonArray and adds a reference at the end of the array.
  // It's a shortcut for JsonBuffer::createArray() and JsonArray::add()
  JsonArray& createNestedArray();

  // Creates a JsonObject and adds a reference at the end of the array.
  // It's a shortcut for JsonBuffer::createObject() and JsonArray::add()
  JsonObject& createNestedObject();

  // Removes element at specified index.
  void remove(size_t index) {
    remove(begin() + index);
  }

  //
  // void remove(const_iterator)
  // void remove(iterator)
  using Internals::List<JsonVariant>::remove;

  // Returns a reference an invalid JsonArray.
  // This object is meant to replace `_buffer(nullptr)`
  // when memory allocation or JSON parsing fail.
  static JsonArray& invalid() {
    static JsonArray instance(Internals::EmptyJsonBuffer::instance());
    return instance;
  }

  // Imports a 1D array
  template <typename T, size_t N>
  bool copyFrom(T (&array)[N]) {
    return copyFrom(array, N);
  }

  // Imports a 1D array
  template <typename T>
  bool copyFrom(T *array, size_t len) {
    bool ok = true;
    for (size_t i = 0; i < len; i++) {
      ok &= add(array[i]);
    }
    return ok;
  }

  // Imports a 2D array
  template <typename T, size_t N1, size_t N2>
  bool copyFrom(T (&array)[N1][N2]) {
    bool ok = true;
    for (size_t i = 0; i < N1; i++) {
      JsonArray& nestedArray = createNestedArray();
      for (size_t j = 0; j < N2; j++) {
        ok &= nestedArray.add(array[i][j]);
      }
    }
    return ok;
  }

  // Exports a 1D array
  template <typename T, size_t N>
  size_t copyTo(T (&array)[N]) const {
    return copyTo(array, N);
  }

  // Exports a 1D array
  template <typename T>
  size_t copyTo(T *array, size_t len) const {
    size_t i = 0;
    for (auto it = begin(); it != end() && i < len; ++it)
      array[i++] = *it;
    return i;
  }

  // Exports a 2D array
  template <typename T, size_t N1, size_t N2>
  void copyTo(T (&array)[N1][N2]) const {
    size_t i = 0;
    for (auto it = begin(); it != end() && i < N1; ++it) {
      it->as<const JsonArray&>().copyTo(array[i++]);
    }
  }

 private:
  template <typename TValue>
  bool set_impl(size_t index, TValue value) {
    iterator it = begin() + index;
    if (it != end())
      return Internals::ValueSaver<TValue>::save(_buffer, *it, std::move(value));

    return false;
  }

  template <typename TValue>
  bool add_impl(TValue value) {
    auto it = Internals::List<JsonVariant>::add();
    if (it != end()) {
      if (Internals::ValueSaver<TValue>::save(_buffer, *it, std::move(value)))
        return true;

      remove(it);
    }

    return false;
  }

  friend class Internals::JsonMutableArraySubscript;
  friend class Internals::JsonConstArraySubscript;
};

namespace Internals {

template <>
struct JsonVariantDefault<JsonArray> {
  static JsonArray& get() {
    return JsonArray::invalid();
  }
};

template <>
struct JsonVariantDefault<const JsonArray> {
  static const JsonArray& get() {
    return JsonArray::invalid();
  }
};

template <>
struct JsonVariantDefault<JsonArray&> {
  static JsonArray& get() {
    return JsonArray::invalid();
  }
};

template <>
struct JsonVariantDefault<const JsonArray&> {
  static const JsonArray& get() {
    return JsonArray::invalid();
  }
};

}  // namespace Internals
}  // namespace ArduinoJson
