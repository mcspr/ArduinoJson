// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Variant/VariantCompare.hpp>
#include <ArduinoJson/Variant/VariantImpl.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename TAdaptedString>
inline VariantData* VariantImpl::getMember(TAdaptedString key) const {
  auto it = findKey(key);
  if (it.done())
    return nullptr;
  it.next(resources_);
  return it.data();
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
  for (auto it = createIterator(); !it.done(); it.next(resources_)) {
    if (isKey &&
        stringEquals(key, adaptString(it.value(resources_).asString())))
      return it;
    isKey = !isKey;
  }
  return iterator();
}

template <typename TAdaptedString>
inline void VariantImpl::removeMember(TAdaptedString key) {
  removeMember(findKey(key));
}

template <typename TAdaptedString>
inline VariantData* VariantImpl::addMember(TAdaptedString key) {
  if (!isObject())
    return nullptr;

  auto keySlot = allocVariant();
  if (!keySlot)
    return nullptr;

  auto valueSlot = allocVariant();
  if (!valueSlot)
    return nullptr;

  VariantImpl keyImpl(keySlot.ptr(), resources_);
  if (!keyImpl.setString(key))
    return nullptr;

  VariantImpl::appendPair(keySlot, valueSlot);

  return valueSlot.ptr();
}

inline VariantData* VariantImpl::addPair(VariantData** value) {
  ARDUINOJSON_ASSERT(isObject());

  auto keySlot = allocVariant();
  if (!keySlot)
    return nullptr;

  auto valueSlot = allocVariant();
  if (!valueSlot)
    return nullptr;
  *value = valueSlot.ptr();

  VariantImpl::appendPair(keySlot, valueSlot);

  return keySlot.ptr();
}

// Returns the size (in bytes) of an object with n members.
constexpr size_t sizeofObject(size_t n) {
  return 2 * n * sizeof(VariantData);
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
