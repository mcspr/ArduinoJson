// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "ListNode.hpp"

#include "../TypeTraits/Constant.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/RemoveConst.hpp"

#include <memory>
#include <cstddef>

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

template <typename, typename>
struct IsListConstConvertible : FalseType {
};

template <typename T>
struct IsListConstConvertible<T const, T> : TrueType {
};

template <typename T>
struct IsListConstConvertible<T const, T const> : TrueType {
};

template <typename T>
struct ListPointerType {
  using type = T*;
};

template <typename T>
struct ListPointerType<T const> {
  using type = const T*;
};

template <typename T>
struct ListReferenceType {
  using type = T&;
};

template <typename T>
struct ListReferenceType<T const> {
  using type = const T&;
};

}

template <typename T>
class List;

// A forward iterator for List<T>, allowing both const or non-const node access
template <typename T>
class ListIterator {
 public:
  using node_type = ListNode<typename RemoveConst<T>::type>;
  using pointer_type = typename TypeTraits::ListPointerType<T>::type;
  using reference_type = typename TypeTraits::ListReferenceType<T>::type;

 private:
  template <typename TOther>
  using is_const_convertible = typename TypeTraits::IsListConstConvertible<T, TOther>::type;

 public:
  ListIterator() = delete;
  explicit ListIterator(node_type *node) :
    _node(node)
  {}

  template <typename TOther, typename EnableIf<is_const_convertible<TOther>::value>::type* = nullptr>
  ListIterator(const ListIterator<TOther> &other) :
    _node(const_cast<node_type *>(other._node))
  {}

  template <typename TOther, typename EnableIf<is_const_convertible<TOther>::value>::type* = nullptr>
  ListIterator& operator=(const ListIterator<TOther> &other) {
    _node = const_cast<node_type *>(other._node);
    return *this;
  }

  ListIterator(const ListIterator<T> &other) = default;
  ListIterator<T>& operator=(const ListIterator<T> &other) = default;

  ListIterator(ListIterator<T> &&other) = default;
  ListIterator<T>& operator=(ListIterator<T> &&other) = default;

  reference_type operator*() const noexcept {
    return _node->content;
  }

  pointer_type operator->() const noexcept {
    return std::addressof(_node->content);
  }

  bool operator==(const ListIterator<T> &other) const noexcept {
    return _node == other._node;
  }

  bool operator!=(const ListIterator<T> &other) const noexcept {
    return _node != other._node;
  }

  ListIterator<T> &operator++() noexcept {
    if (_node)
      _node = _node->next;
    return *this;
  }

  ListIterator<T> operator++(int) const noexcept {
    ListIterator<T> out(*this);
    ++out;
    return out;
  }

  ListIterator<T> &operator+=(size_t distance) noexcept {
    while (_node && distance) {
      _node = _node->next;
      --distance;
    }
    return *this;
  }

  ListIterator<T> operator+(size_t distance) const noexcept {
    ListIterator<T> out(*this);
    out += distance;
    return out;
  }

 private:
  template <typename Node>
  friend class List;

  template <typename Other>
  friend class ListIterator;

  node_type *_node;
};

}  // namespace Internals
}  // namespace ArduinoJson
