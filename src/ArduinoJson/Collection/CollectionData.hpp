// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Memory/MemoryPool.hpp>
#include <ArduinoJson/Namespace.hpp>
#include <ArduinoJson/Polyfills/assert.hpp>

#include <stddef.h>  // size_t

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

struct VariantData;
class ResourceManager;

class CollectionIterator {
  friend class CollectionImpl;

 public:
  CollectionIterator() : slot_(nullptr), currentId_(NULL_SLOT) {}

  void next(const ResourceManager* resources);

  bool done() const {
    return slot_ == nullptr;
  }

  bool operator==(const CollectionIterator& other) const {
    return slot_ == other.slot_;
  }

  bool operator!=(const CollectionIterator& other) const {
    return slot_ != other.slot_;
  }

  VariantData* operator->() {
    ARDUINOJSON_ASSERT(slot_ != nullptr);
    return data();
  }

  VariantData& operator*() {
    ARDUINOJSON_ASSERT(slot_ != nullptr);
    return *data();
  }

  const VariantData& operator*() const {
    ARDUINOJSON_ASSERT(slot_ != nullptr);
    return *data();
  }

  VariantData* data() {
    return slot_;
  }

  const VariantData* data() const {
    return slot_;
  }

 private:
  CollectionIterator(VariantData* slot, SlotId slotId)
      : slot_(slot), currentId_(slotId) {}

  VariantData* slot_;
  SlotId currentId_;
};

class CollectionImpl {
 protected:
  VariantData* data_;
  ResourceManager* resources_;

 public:
  using iterator = CollectionIterator;

  CollectionImpl() : data_(nullptr), resources_(nullptr) {}

  CollectionImpl(VariantData* data, ResourceManager* resources)
      : data_(data), resources_(resources) {}

  explicit operator bool() const {
    return data_ && data_->isCollection();
  }

  bool isNull() const {
    return !operator bool();
  }

  VariantData* getData() const {
    return data_;
  }

  ResourceManager* getResourceManager() const {
    return resources_;
  }

  iterator createIterator() const {
    if (isNull())
      return iterator();
    return createIterator(data_, resources_);
  }

  static iterator createIterator(VariantData*, ResourceManager*);

  size_t size() const;
  size_t nesting() const;

  void clear() {
    if (isNull())
      return;
    clear(data_, resources_);
  }

  static void clear(VariantData*, ResourceManager*);

  SlotId head() const {
    ARDUINOJSON_ASSERT(data_ != nullptr);
    ARDUINOJSON_ASSERT(data_->isCollection());
    return data_->content.asCollection.head;
  }

  static void appendOne(Slot<VariantData> slot, VariantData*, ResourceManager*);

  static void appendPair(Slot<VariantData> key, Slot<VariantData> value,
                         VariantData*, ResourceManager*);

  void removeOne(iterator it);
  void removePair(iterator it);

 private:
  Slot<VariantData> getPreviousSlot(VariantData*) const;
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
