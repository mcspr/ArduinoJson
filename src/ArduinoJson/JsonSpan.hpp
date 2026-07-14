// MIT License

#pragma once

#include "Polyfills/math.hpp"

#include "TypeTraits/IsChar.hpp"
#include "TypeTraits/RemoveConst.hpp"

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

static constexpr auto JsonSpanDynamicExtent = size_t(-1);

template <size_t Size>
struct ExtentImpl {
  constexpr ExtentImpl() = default;
  constexpr ExtentImpl(size_t) {
  }

  constexpr size_t size() const {
    return Size;
  }
};

template <>
struct ExtentImpl<JsonSpanDynamicExtent> {
  ExtentImpl() = delete;

  constexpr ExtentImpl(size_t extent_size) :
    _size(extent_size)
  {}

  constexpr size_t size() const {
    return _size;
  }

private:
 size_t _size;
};

template <typename T, size_t Extent = JsonSpanDynamicExtent>
struct JsonSpan {
  static_assert(Internals::IsChar<
    typename Internals::RemoveConst<T>::type>::value, "");

  constexpr explicit JsonSpan(std::nullptr_t) :
    _data(nullptr)
  {}

  constexpr JsonSpan() noexcept :
    JsonSpan(nullptr)
  {}

  constexpr explicit JsonSpan(T* span_data) :
    _data(span_data)
  {}

  constexpr explicit JsonSpan(std::nullptr_t, size_t) :
    _data(nullptr),
    _extent(0)
  {}

  constexpr explicit JsonSpan(T* span_data, size_t extent_size) :
    _data(span_data),
    _extent(extent_size)
  {}

  constexpr T* data() const {
    return _data;
  }

  T* data() {
    return _data;
  }

  constexpr bool is_null() const {
    return _data == nullptr;
  }

  constexpr bool empty() const {
    return is_null() || _extent.size == 0;
  }

  constexpr size_t size() const {
    return _extent.size();
  }

  constexpr T* begin() const {
    return _data;
  }

  constexpr T* end() const {
    return _data + size();
  }

  constexpr T& operator[](size_t index) const {
    return _data[index];
  }

  constexpr T& front() const {
    return _data[0];
  }

  constexpr T& back() const {
    return _data[size() ? size() - 1 : 0];
  }

  constexpr JsonSpan<T> slice(size_t index, size_t extent_size) const {
    return JsonSpan(
      _data + Min(index, this->size()),
      Min(extent_size, this->size() - index));
  }

  constexpr JsonSpan<T> slice(size_t index) const {
    return slice(index, size() - index);
  }

  JsonSpan<T>& advance(size_t index) {
    return *this = slice(index);
  }

private:
  T* _data;
  ExtentImpl<Extent> _extent;
};

} // namespace Internals

template <typename T>
using JsonDynamicSpan = Internals::JsonSpan<T, Internals::JsonSpanDynamicExtent>;

template <typename T>
constexpr JsonDynamicSpan<T> JsonSpan(T* data, size_t size) {
  return JsonDynamicSpan<T>(data, size);
}

template <typename T, size_t Size>
using JsonStaticSpan = Internals::JsonSpan<T, Size>;

template <typename T, size_t Size>
constexpr JsonStaticSpan<T, Size> JsonSpan(T (&data)[Size]) {
  return JsonStaticSpan<T, Size>(&data[0]);
}

} // namespace ArduinoJson
