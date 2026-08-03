// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Data/JsonVariantAs.hpp"
#include "JsonSubscriptBase.hpp"

namespace ArduinoJson {
namespace Internals {

class JsonMutableArraySubscript;
class JsonConstArraySubscript;

class JsonMutableArraySubscript final :
    public JsonSubscriptBase<JsonArray, JsonMutableArraySubscript> {

 public:
  JsonMutableArraySubscript() noexcept;
  JsonMutableArraySubscript(JsonArray&, size_t index) noexcept;

  // allow to construct the object, but disallow changes after construction

  JsonMutableArraySubscript(const JsonMutableArraySubscript &) = default;
  JsonMutableArraySubscript(JsonMutableArraySubscript &&) = default;

  JsonMutableArraySubscript(const JsonConstArraySubscript &) = delete;
  JsonMutableArraySubscript(JsonConstArraySubscript &&) = delete;

  // class copy / move implicitly converts into JsonVariant and assigns

  JsonMutableArraySubscript& operator=(const JsonMutableArraySubscript&);
  JsonMutableArraySubscript& operator=(JsonMutableArraySubscript&&);

  JsonMutableArraySubscript& operator=(const JsonConstArraySubscript&);
  JsonMutableArraySubscript& operator=(JsonConstArraySubscript&&);

  template <typename TValue>
  JsonMutableArraySubscript& operator=(TValue&&);

  // forwarding methods for the bound index and JsonArray ref

  bool success() const;

  // aka JsonArray::get<TValue>(index)
  template <typename TValue>
  typename JsonVariantAs<TValue>::type
  as() const;

  // aka JsonArray::is<TValue>(index)
  template <typename TValue>
  bool is() const;

  // aka JsonArray::set<TValue>(index, value)
  template <typename TValue>
  bool set(TValue&& value);

 private:
  JsonArray& _array;
  size_t _index{};

  friend class JsonConstArraySubscript;
};

class JsonConstArraySubscript final :
    public JsonSubscriptBase<const JsonArray, JsonConstArraySubscript> {

 public:
  JsonConstArraySubscript() noexcept;
  JsonConstArraySubscript(const JsonArray&, size_t index) noexcept;

  // allow to construct the object, but disallow changes after construction

  JsonConstArraySubscript(const JsonConstArraySubscript &) noexcept;
  JsonConstArraySubscript(JsonConstArraySubscript &&) noexcept;

  JsonConstArraySubscript(const JsonMutableArraySubscript &) noexcept;
  JsonConstArraySubscript(JsonMutableArraySubscript &&) noexcept;

  // class copy / move cannot assign anything to a cref

  JsonConstArraySubscript& operator=(const JsonConstArraySubscript&) = delete;
  JsonConstArraySubscript& operator=(JsonConstArraySubscript&&) = delete;

  JsonConstArraySubscript& operator=(const JsonMutableArraySubscript&) = delete;
  JsonConstArraySubscript& operator=(JsonMutableArraySubscript&&) = delete;

  template <typename TValue>
  JsonConstArraySubscript& operator=(TValue&&) = delete;

  // forwarding methods for the bound index and JsonArray ref

  bool success() const;

  // aka JsonArray::get<TValue>(index)
  template <typename TValue>
  typename JsonVariantAsConst<TValue>::type
  as() const;

  // aka JsonArray::is<TValue>(index)
  template <typename TValue>
  bool is() const;

 private:
  const JsonArray& _array;
  size_t _index{};

  friend class JsonMutableArraySubscript;
};

}  // namespace Internals
}  // namespace ArduinoJson
