// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Array/ArrayData.hpp>
#include <ArduinoJson/Variant/VariantCompare.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

inline ArrayImpl::iterator ArrayImpl::at(size_t index) const {
  if (isNull())
    return iterator();

  auto it = createIterator();
  while (!it.done() && index) {
    it.next(resources_);
    --index;
  }
  return it;
}

inline VariantData* ArrayImpl::addElement(VariantData* data,
                                          ResourceManager* resources) {
  ARDUINOJSON_ASSERT(data != nullptr);
  ARDUINOJSON_ASSERT(data->isArray());
  ARDUINOJSON_ASSERT(resources != nullptr);

  auto slot = resources->allocVariant();
  if (!slot)
    return nullptr;
  CollectionImpl::appendOne(slot, data, resources);
  return slot.ptr();
}

inline VariantData* ArrayImpl::getOrAddElement(size_t index) {
  auto it = createIterator();
  while (!it.done() && index > 0) {
    it.next(resources_);
    index--;
  }
  if (it.done())
    index++;
  VariantData* element = it.data();
  while (index > 0) {
    element = addElement();
    if (!element)
      return nullptr;
    index--;
  }
  return element;
}

inline VariantData* ArrayImpl::getElement(size_t index) const {
  return at(index).data();
}

inline void ArrayImpl::removeElement(size_t index) {
  remove(at(index));
}

template <typename T>
inline bool ArrayImpl::addValue(const T& value, VariantData* data,
                                ResourceManager* resources) {
  ARDUINOJSON_ASSERT(data != nullptr);
  ARDUINOJSON_ASSERT(resources != nullptr);
  auto slot = resources->allocVariant();
  if (!slot)
    return false;
  JsonVariant variant(slot.ptr(), resources);
  if (!variant.set(value)) {
    resources->freeVariant(slot);
    return false;
  }
  CollectionImpl::appendOne(slot, data, resources);
  return true;
}

// Returns the size (in bytes) of an array with n elements.
constexpr size_t sizeofArray(size_t n) {
  return n * sizeof(VariantData);
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
