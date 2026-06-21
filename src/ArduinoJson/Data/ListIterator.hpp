// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "ListNode.hpp"
#include "../TypeTraits/IsConst.hpp"
#include "../TypeTraits/IsSame.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/And.hpp"
#include "../TypeTraits/RemoveConst.hpp"
#include "../TypeTraits/Conditional.hpp"

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
  using pointer_type = ConditionalType<IsConst<T>::value, const value_type*, value_type*>;
  using reference_type = ConditionalType<IsConst<T>::value, const value_type&, value_type&>;

  ListIterator() = delete;
  explicit ListIterator(ListNode<value_type> *node) :
    _node(node)
  {}

  template <typename Other>
  ListIterator(const ListIterator<Other> &other,
    typename EnableIf<And<IsSame<typename RemoveConst<T>::type, typename RemoveConst<Other>::type>,
                 IsConst<T>>::value>::type* = nullptr) :
    _node(const_cast<ListNode<value_type> *>(other._node))
  {}

  template <typename Other>
  typename EnableIf<And<IsSame<typename RemoveConst<T>::type, typename RemoveConst<Other>::type>,
               IsConst<T>>::value, ListIterator<T> &>::type
  operator=(const ListIterator<Other> &other) {
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
