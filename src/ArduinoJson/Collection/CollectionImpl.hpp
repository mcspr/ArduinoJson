// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Collection/CollectionIterator.hpp>
#include <ArduinoJson/Memory/Alignment.hpp>
#include <ArduinoJson/Strings/StringAdapters.hpp>
#include <ArduinoJson/Variant/VariantCompare.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

inline void CollectionIterator::next() {
  ARDUINOJSON_ASSERT(slot_);
  auto nextId = slot_->next;
  slot_ = resources_->getVariant(nextId);
  currentId_ = nextId;
}

inline VariantImpl CollectionIterator::value() const {
  ARDUINOJSON_ASSERT(slot_ != nullptr);
  return VariantImpl(slot_, resources_);
}

inline VariantImpl::iterator VariantImpl::createIterator() const {
  if (!data_ || !data_->isCollection())
    return iterator();
  auto coll = getCollectionData();
  return iterator(coll->head, resources_);
}

inline void VariantImpl::appendOne(Slot<VariantData> slot) {
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

inline void VariantImpl::appendPair(Slot<VariantData> key,
                                    Slot<VariantData> value) {
  auto coll = getCollectionData();

  key->next = value.id();

  if (coll->tail != NULL_SLOT) {
    auto tail = getVariant(coll->tail);
    tail->next = key.id();
    coll->tail = value.id();
  } else {
    coll->head = key.id();
    coll->tail = value.id();
  }
}
inline Slot<VariantData> VariantImpl::getPreviousSlot(
    VariantData* target) const {
  auto coll = getCollectionData();
  auto prev = Slot<VariantData>();
  auto currentId = coll->head;
  while (currentId != NULL_SLOT) {
    auto currentSlot = getVariant(currentId);
    if (currentSlot == target)
      break;
    prev = Slot<VariantData>(currentSlot, currentId);
    currentId = currentSlot->next;
  }
  return prev;
}

inline void VariantImpl::removeOne(iterator it) {
  if (it.done())
    return;
  auto coll = getCollectionData();
  auto curr = it.slot_;
  auto prev = getPreviousSlot(curr);
  auto next = curr->next;
  if (prev)
    prev->next = next;
  else
    coll->head = next;
  if (next == NULL_SLOT)
    coll->tail = prev.id();
  freeVariant({it.slot_, it.currentId_});
}

inline void VariantImpl::removePair(VariantImpl::iterator it) {
  if (it.done())
    return;

  auto keySlot = it.slot_;

  auto valueId = keySlot->next;
  auto valueSlot = getVariant(valueId);

  // remove value slot
  keySlot->next = valueSlot->next;
  freeVariant({valueSlot, valueId});

  // remove key slot
  removeOne(it);
}

inline size_t VariantImpl::nesting() const {
  if (!data_ || !data_->isCollection())
    return 0;
  size_t maxChildNesting = 0;
  for (auto it = createIterator(); !it.done(); it.next()) {
    auto childNesting = it.value().nesting();
    if (childNesting > maxChildNesting)
      maxChildNesting = childNesting;
  }
  return maxChildNesting + 1;
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
