// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Namespace.hpp>
#include <ArduinoJson/Polyfills/assert.hpp>

#include <stddef.h>  // size_t

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

struct VariantData;
class ResourceManager;
class VariantImpl;

class CollectionIterator {
  friend class VariantImpl;

 public:
  CollectionIterator() {}

  void next();

  VariantImpl value() const;

  bool done() const {
    return slot_ == nullptr;
  }

  bool operator==(const CollectionIterator& other) const {
    return slot_ == other.slot_;
  }

  bool operator!=(const CollectionIterator& other) const {
    return slot_ != other.slot_;
  }

  VariantData* data() {
    return slot_;
  }

  const VariantData* data() const {
    return slot_;
  }

 private:
  CollectionIterator(SlotId slotId, ResourceManager* resources)
      : slot_(resources->getVariant(slotId)),
        currentId_(slotId),
        resources_(resources) {}

  VariantData* slot_ = nullptr;
  SlotId currentId_ = NULL_SLOT;
  ResourceManager* resources_ = nullptr;
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
