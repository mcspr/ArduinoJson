// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Namespace.hpp>
#include <ArduinoJson/Polyfills/assert.hpp>
#include <ArduinoJson/Variant/VariantImpl.hpp>

#include <stddef.h>  // size_t

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

struct VariantData;
class ResourceManager;
class VariantImpl;

class CollectionIterator {
  friend class VariantImpl;

 public:
  CollectionIterator() {}

  void next() {
    ARDUINOJSON_ASSERT(!done());
    auto nextId = value_.data()->next;
    auto resources = value_.resources();
    value_ = VariantImpl(resources->getVariant(nextId), resources);
    currentId_ = nextId;
  }

  const VariantImpl& operator*() const {
    return value_;
  }

  const VariantImpl* operator->() const {
    return &value_;
  }

  bool done() const {
    return value_.isUnbound();
  }

  bool operator==(const CollectionIterator& other) const {
    return value_.data() == other->data();
  }

  bool operator!=(const CollectionIterator& other) const {
    return !operator==(other);
  }

 private:
  CollectionIterator(SlotId slotId, ResourceManager* resources)
      : value_(resources->getVariant(slotId), resources), currentId_(slotId) {}

  VariantImpl value_;
  SlotId currentId_ = NULL_SLOT;
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
