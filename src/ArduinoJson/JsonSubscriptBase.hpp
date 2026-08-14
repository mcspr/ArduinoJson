// MIT License

#pragma once

#include "JsonImplicitConversions.hpp"

#include "JsonVariantComparisons.hpp"
#include "JsonVariantOr.hpp"

#include "Serialization/JsonPrintable.hpp"

#include "TypeTraits/IsSubscript.hpp"

#include "StringTraits/StringTraits.hpp"
#include "Data/StringViewRef.hpp"

namespace ArduinoJson {
namespace Internals {

// respect subscript member constness, not the object itself
// for non-const, explicitly sets operator= and operator T() conversions
// for const, deletes them (xxx: note that base class *may* keep copy operators, take care to delete those)

template <typename T, typename TImpl>
struct JsonSubscriptConversions {
  using type = JsonImplicitConversions<TImpl, JsonImplicitAnyReference>;
};

template <typename T, typename TImpl>
struct JsonSubscriptConversions<T const, TImpl> {
  using type = JsonImplicitConversions<TImpl, JsonImplicitConstReference>;
};

// allow subscript operator[] chaining
// same as originating objects, only non-const -> const and const -> const chains allowed
// note that subscript chain *does not* create elements, only references existing ones

class JsonMutableArraySubscript;

template <typename TKey>
struct TJsonMutableObjectSubscriptType;

template <typename TImpl>
class JsonMutableSubscripts {
 public:
  // aka JsonArray::operator[](index);
  JsonMutableArraySubscript
  operator[](size_t) const;

  // aka JsonObject::operator[](key);
  template <typename TKey>
  typename EnableIf<HasStringTraits<TKey>::value,
    typename TJsonMutableObjectSubscriptType<TKey>::subscript_type>::type
  operator[](TKey&&) const;

 private:
  const TImpl *impl() const {
    return static_cast<const TImpl *>(this);
  }
};

class JsonConstArraySubscript;

template <typename TKey>
struct TJsonConstObjectSubscriptType;

template <typename TImpl>
class JsonConstSubscripts {
 public:
  // aka JsonArray::operator[](index) const;
  JsonConstArraySubscript
  operator[](size_t) const;

  // aka JsonObject::operator[](key) const;
  template <typename TKey>
  typename EnableIf<HasStringTraits<TKey>::value,
    typename TJsonConstObjectSubscriptType<TKey>::subscript_type>::type
  operator[](TKey&&) const;

 private:
  const TImpl *impl() const {
    return static_cast<const TImpl *>(this);
  }
};

template <typename T, typename TImpl>
struct JsonSubscriptSubscripts {
  using type = JsonMutableSubscripts<TImpl>;
};

template <typename T, typename TImpl>
struct JsonSubscriptSubscripts<T const, TImpl> {
  using type = JsonConstSubscripts<TImpl>;
};

// subscript types inherit from this instead of variant base,
// but are expected to keep other JsonVariant properties
template <typename T, typename TImpl>
class JsonSubscriptBase :
    public JsonPrintable<TImpl>,
    public JsonSubscriptConversions<T, TImpl>::type,
    public JsonSubscriptSubscripts<T, TImpl>::type,
    public JsonVariantComparisons<TImpl>,
    public JsonVariantOr<TImpl>,
    public JsonSubscriptTag,
    public JsonVariantTag {

};

}  // namespace Internals
}  // namespace ArduinoJson
