// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Variant/VariantCompare.hpp>
#include <ArduinoJson/Variant/VariantImpl.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

inline bool VariantImpl::copyObject(const VariantImpl& src) {
  ARDUINOJSON_ASSERT(isNull());

  if (!data_)
    return false;

  data_->toObject();

  for (auto it = src.createIterator(); !it.done(); it.move()) {
    auto keySlot = allocVariant();
    if (!keySlot)
      return false;

    auto key = VariantImpl(keySlot.ptr(), resources_);
    if (!key.copyVariant(*it)) {
      freeVariant(keySlot);
      return false;
    }

    it.move();  // move to value
    ARDUINOJSON_ASSERT(!it.done());

    auto valueSlot = allocVariant();
    if (!valueSlot) {
      freeVariant(keySlot);
      return false;
    }

    // TODO: we add the pair before copying the value to be keep the old
    // behavior but this is not consistent with issue #2081
    addMember(keySlot, valueSlot);

    auto value = VariantImpl(valueSlot.ptr(), resources_);
    if (!value.copyVariant(*it))
      return false;
  }

  return true;
}

template <typename TAdaptedString>
inline VariantData* VariantImpl::getMember(TAdaptedString key) const {
  auto it = findKey(key);
  if (it.done())
    return nullptr;
  it.move();
  return it->data();
}

template <typename TAdaptedString>
VariantData* VariantImpl::getOrAddMember(TAdaptedString key) {
  auto data = getMember(key);
  if (data)
    return data;
  return addMember(key);
}

template <typename TAdaptedString>
inline VariantImpl::iterator VariantImpl::findKey(TAdaptedString key) const {
  if (!isObject())
    return iterator();
  if (key.isNull())
    return iterator();
  bool isKey = true;
  for (auto it = createIterator(); !it.done(); it.move()) {
    if (isKey && stringEquals(key, adaptString(it->asString())))
      return it;
    isKey = !isKey;
  }
  return iterator();
}

template <typename TAdaptedString>
inline void VariantImpl::removeMember(TAdaptedString key) {
  removeMember(findKey(key));
}

inline void VariantImpl::removeMember(iterator it) {
  removePair(it);
}

template <typename TAdaptedString>
inline VariantData* VariantImpl::addMember(TAdaptedString key) {
  if (!isObject())
    return nullptr;

  if (key.isNull())
    return nullptr;  // Ignore null key

  auto keySlot = allocVariant();
  if (!keySlot)
    return nullptr;

  auto valueSlot = allocVariant();
  if (!valueSlot)
    return nullptr;

  VariantImpl keyImpl(keySlot.ptr(), resources_);
  if (!keyImpl.setString(key))
    return nullptr;

  addMember(keySlot, valueSlot);

  return valueSlot.ptr();
}

// Returns the size (in bytes) of an object with n members.
constexpr size_t sizeofObject(size_t n) {
  return 2 * n * sizeof(VariantData);
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
