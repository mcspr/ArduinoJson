// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Collection/CollectionIterator.hpp>
#include <ArduinoJson/Variant/VariantImpl.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

inline VariantImpl::iterator VariantImpl::at(size_t index) const {
  if (!isArray())
    return iterator();

  auto it = createIterator();
  while (!it.done() && index) {
    it.move();
    --index;
  }
  return it;
}

inline VariantData* VariantImpl::addElement() {
  if (!isArray())
    return nullptr;
  auto slot = allocVariant();
  if (!slot)
    return nullptr;
  addElement(slot);
  return slot.ptr();
}

inline void VariantImpl::addElement(Slot<VariantData> slot) {
  auto coll = getCollectionData();

  if (coll->tail != NULL_SLOT) {
    auto tail = getVariant(coll->tail);
    tail->next = slot.id();
    coll->tail = slot.id();
  } else {
    coll->head = slot.id();
    coll->tail = slot.id();
  }
}

inline VariantData* VariantImpl::getOrAddElement(size_t index) {
  auto it = createIterator();
  while (!it.done() && index > 0) {
    it.move();
    index--;
  }
  if (it.done())
    index++;
  VariantData* element = it->data();
  while (index > 0) {
    element = addElement();
    if (!element)
      return nullptr;
    index--;
  }
  return element;
}

inline VariantData* VariantImpl::getElement(size_t index) const {
  return at(index)->data();
}

inline void VariantImpl::removeElement(iterator it) {
  if (!isArray())
    return;
  removeOne(it);
}

inline void VariantImpl::removeElement(size_t index) {
  removeElement(at(index));
}

// Returns the size (in bytes) of an array with n elements.
constexpr size_t sizeofArray(size_t n) {
  return n * sizeof(VariantData);
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
