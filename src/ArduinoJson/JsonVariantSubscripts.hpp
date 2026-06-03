// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Data/JsonVariantAs.hpp"
#include "Polyfills/attributes.hpp"
#include "StringTraits/StringTraits.hpp"
#include "TypeTraits/EnableIf.hpp"

namespace ArduinoJson {
namespace Internals {

// Forward declarations.
class JsonArraySubscript;
template <typename TKey>
class JsonObjectSubscript;

template <typename TImpl>
class JsonVariantSubscripts {
 public:
  // Mimics an array or an object.
  // Returns the size of the array or object if the variant has that type.
  // Returns 0 if the variant is neither an array nor an object
  size_t size() const {
    return impl()->template as<JsonArray>().size() +
           impl()->template as<JsonObject>().size();
  }

  // Mimics an array.
  // Returns the element at specified index if the variant is an array.
  // Returns JsonVariant::invalid() if the variant is not an array.
  ARDUINOJSON_FORCE_INLINE const JsonArraySubscript operator[](size_t index) const;
  ARDUINOJSON_FORCE_INLINE JsonArraySubscript operator[](size_t index);

  // Mimics an object.
  // Returns the value associated with the specified key if the variant is
  // an object.
  // Return JsonVariant::invalid() if the variant is not an object.
  //
  // const JsonObjectSubscript operator[](TKey) const;
  // TKey = const std::string&, const String&
  template <typename TKey>
  ARDUINOJSON_FORCE_INLINE
      typename EnableIf<StringTraits<TKey>::has_equals,
                        const JsonObjectSubscript<TKey> >::type
      operator[](TKey &&key) const {
    return impl()->template as<JsonObject>()[std::forward<TKey>(key)];
  }

  template <typename TChar, size_t Size>
  ARDUINOJSON_FORCE_INLINE
      typename EnableIf<StringTraits<TChar[Size]>::has_equals,
                        const JsonObjectSubscript<TChar*> >::type
      operator[](TChar (&key)[Size]) const {
    return impl()->template as<JsonObject>()[key];
  }
  //
  // const JsonObjectSubscript operator[](TKey) const;
  // TKey = const std::string&, const String&
  template <typename TKey>
  ARDUINOJSON_FORCE_INLINE typename EnableIf<StringTraits<TKey>::has_equals,
                                 JsonObjectSubscript<TKey> >::type
  operator[](TKey &&key) {
    return impl()->template as<JsonObject>()[std::forward<TKey>(key)];
  }

  template <typename TChar, size_t Size>
  ARDUINOJSON_FORCE_INLINE typename EnableIf<StringTraits<TChar[Size]>::has_equals,
                                 JsonObjectSubscript<TChar*> >::type
  operator[](TChar (&key)[Size]) {
    return impl()->template as<JsonObject>()[key];
  }

 private:
  TImpl *impl() {
    return static_cast<TImpl *>(this);
  }

  const TImpl *impl() const {
    return static_cast<const TImpl *>(this);
  }
};
}  // namespace Internals
}  // namespace ArduinoJson
