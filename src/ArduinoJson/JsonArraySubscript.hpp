// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Data/JsonVariantAs.hpp"
#include "JsonVariantBase.hpp"

namespace ArduinoJson {
namespace Internals {
class JsonArraySubscript : public JsonVariantBase<JsonArraySubscript> {
 public:
  JsonArraySubscript() noexcept;
  JsonArraySubscript(JsonArray& array, size_t index) noexcept;

  // Allow to construct the object, but disallow changes after construction

  JsonArraySubscript(const JsonArraySubscript &) = default;
  JsonArraySubscript(JsonArraySubscript &&) = default;

  // Everything else is interpreted as array assignment w/ the index attached to the subscript object
  JsonArraySubscript& operator=(const JsonArraySubscript&);

  template <typename TValue>
  JsonArraySubscript& operator=(TValue&& value);

  // forwarding methods for the bound index and JsonArray ref
  bool success() const;

  template <typename TValue>
  typename JsonVariantAs<TValue>::type
  as() const;

  template <typename T>
  bool is() const;

  // Replaces the value
  template <typename TValue>
  bool set(TValue&& value);

 private:
  JsonArray& _array;
  size_t _index{};
};

template <typename TImpl>
inline JsonArraySubscript JsonVariantSubscripts<TImpl>::operator[](size_t index) {
  return impl()->template as<JsonArray>()[index];
}

template <typename TImpl>
inline JsonArraySubscript
ARDUINOJSON_FORCE_INLINE JsonVariantSubscripts<TImpl>::operator[](size_t index) const {
  return impl()->template as<JsonArray>()[index];
}

#if ARDUINOJSON_ENABLE_STD_STREAM
inline std::ostream& operator<<(std::ostream& os,
                                const JsonArraySubscript& source) {
  return source.printTo(os);
}
#endif
}  // namespace Internals
}  // namespace ArduinoJson
