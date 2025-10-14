// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Collection/CollectionData.hpp>
#include <ArduinoJson/Memory/Alignment.hpp>
#include <ArduinoJson/Strings/StringAdapters.hpp>
#include <ArduinoJson/Variant/VariantCompare.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

inline void CollectionIterator::next(const ResourceManager* resources) {
  ARDUINOJSON_ASSERT(slot_);
  auto nextId = slot_->next;
  slot_ = resources->getVariant(nextId);
  currentId_ = nextId;
}

inline CollectionImpl::iterator CollectionImpl::createIterator(
    VariantData* data, ResourceManager* resources) {
  ARDUINOJSON_ASSERT(data != nullptr);
  ARDUINOJSON_ASSERT(data->isCollection());
  ARDUINOJSON_ASSERT(resources != nullptr);
  auto head = data->content.asCollection.head;
  return iterator(resources->getVariant(head), head);
}

inline void CollectionImpl::appendOne(Slot<VariantData> slot, VariantData* data,
                                      ResourceManager* resources) {
  ARDUINOJSON_ASSERT(data != nullptr);
  ARDUINOJSON_ASSERT(data->isCollection());
  ARDUINOJSON_ASSERT(resources != nullptr);

  auto coll = &data->content.asCollection;

  if (coll->tail != NULL_SLOT) {
    auto tail = resources->getVariant(coll->tail);
    tail->next = slot.id();
    coll->tail = slot.id();
  } else {
    coll->head = slot.id();
    coll->tail = slot.id();
  }
}

inline void CollectionImpl::appendPair(Slot<VariantData> key,
                                       Slot<VariantData> value,
                                       VariantData* data,
                                       ResourceManager* resources) {
  ARDUINOJSON_ASSERT(data != nullptr);
  ARDUINOJSON_ASSERT(resources != nullptr);

  key->next = value.id();

  auto coll = &data->content.asCollection;

  if (coll->tail != NULL_SLOT) {
    auto tail = resources->getVariant(coll->tail);
    tail->next = key.id();
    coll->tail = value.id();
  } else {
    coll->head = key.id();
    coll->tail = value.id();
  }
}

inline void CollectionImpl::clear(VariantData* data,
                                  ResourceManager* resources) {
  ARDUINOJSON_ASSERT(data != nullptr);
  ARDUINOJSON_ASSERT(data->isCollection());
  ARDUINOJSON_ASSERT(resources != nullptr);

  auto coll = &data->content.asCollection;

  auto next = coll->head;
  while (next != NULL_SLOT) {
    auto currId = next;
    auto slot = resources->getVariant(next);
    next = slot->next;
    resources->freeVariant({slot, currId});
  }

  coll->head = NULL_SLOT;
  coll->tail = NULL_SLOT;
}

inline Slot<VariantData> CollectionImpl::getPreviousSlot(
    VariantData* target) const {
  ARDUINOJSON_ASSERT(data_ != nullptr);
  ARDUINOJSON_ASSERT(data_->isCollection());
  ARDUINOJSON_ASSERT(resources_ != nullptr);

  auto prev = Slot<VariantData>();
  auto currentId = data_->content.asCollection.head;
  while (currentId != NULL_SLOT) {
    auto currentSlot = resources_->getVariant(currentId);
    if (currentSlot == target)
      break;
    prev = Slot<VariantData>(currentSlot, currentId);
    currentId = currentSlot->next;
  }
  return prev;
}

inline void CollectionImpl::removeOne(iterator it) {
  if (it.done())
    return;
  auto curr = it.slot_;
  auto prev = getPreviousSlot(curr);
  auto next = curr->next;
  auto coll = &data_->content.asCollection;
  if (prev)
    prev->next = next;
  else
    coll->head = next;
  if (next == NULL_SLOT)
    coll->tail = prev.id();
  resources_->freeVariant({it.slot_, it.currentId_});
}

inline void CollectionImpl::removePair(iterator it) {
  if (it.done())
    return;

  auto keySlot = it.slot_;

  auto valueId = keySlot->next;
  auto valueSlot = resources_->getVariant(valueId);

  // remove value slot
  keySlot->next = valueSlot->next;
  resources_->freeVariant({valueSlot, valueId});

  // remove key slot
  removeOne(it);
}

inline size_t CollectionImpl::nesting() const {
  if (!data_ || !data_->isCollection())
    return 0;
  size_t maxChildNesting = 0;
  for (auto it = createIterator(); !it.done(); it.next(resources_)) {
    size_t childNesting = VariantImpl(it.data(), resources_).nesting();
    if (childNesting > maxChildNesting)
      maxChildNesting = childNesting;
  }
  return maxChildNesting + 1;
}

inline size_t CollectionImpl::size() const {
  size_t count = 0;
  for (auto it = createIterator(); !it.done(); it.next(resources_))
    count++;
  return count;
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
