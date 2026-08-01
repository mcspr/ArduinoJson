// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "ListNode.hpp"

#include "../TypeTraits/And.hpp"
#include "../TypeTraits/Conditional.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsConst.hpp"
#include "../TypeTraits/IsSame.hpp"
#include "../TypeTraits/RemoveConst.hpp"

#include <memory>
#include <cstddef>

namespace ArduinoJson {
namespace Internals {

template <typename T>
class List;

// A forward iterator for List<T>, allowing both const or non-const node access
template <typename T>
class ListIterator {
 public:
  using value_type = typename RemoveConst<T>::type;
  using node_type = ListNode<value_type>;
  using pointer_type = Conditional<IsConst<T>::value, const value_type*, value_type*>;
  using reference_type = Conditional<IsConst<T>::value, const value_type&, value_type&>;

 private:
  template <typename TOther>
  using is_const_convertible = And<IsConst<T>, IsSame<typename RemoveConst<TOther>::type, value_type>>;

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
