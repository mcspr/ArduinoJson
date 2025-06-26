// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Memory/ResourceManager.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename T>
inline void VariantImpl::setRawString(SerializedValue<T> value) {
  if (!data_)
    return;
  auto dup = resources_->saveString(adaptString(value.data(), value.size()));
  if (dup)
    data_->setRawString(dup);
}

inline bool VariantImpl::setLinkedString(const char* s) {
  ARDUINOJSON_ASSERT(isNull());  // must call clear() first
  ARDUINOJSON_ASSERT(s);

  auto slotId = resources_->saveStaticString(s);
  if (slotId == NULL_SLOT)
    return false;

  data_->type = VariantType::LinkedString;
  data_->content.asSlotId = slotId;
  return true;
}

template <typename TAdaptedString>
inline bool VariantImpl::setString(TAdaptedString value) {
  ARDUINOJSON_ASSERT(isNull());  // must call clear() first

  if (!data_)
    return false;

  if (value.isNull())
    return false;

  if (value.isStatic())
    return setLinkedString(value.data());

  if (isTinyString(value, value.size())) {
    data_->setTinyString(value);
    return true;
  }

  auto dup = resources_->saveString(value);
  if (dup) {
    data_->setOwnedString(dup);
    return true;
  }

  return false;
}

inline void VariantImpl::clear() {
  if (!data_)
    return;

  if (data_->type & VariantTypeBits::OwnedStringBit)
    resources_->dereferenceString(data_->content.asOwnedString->data);

#if ARDUINOJSON_USE_8_BYTE_POOL
  if (data_->type & VariantTypeBits::EightByteBit)
    resources_->freeEightByte(data_->content.asSlotId);
#endif

  asCollection().clear();

  data_->type = VariantType::Null;
}

#if ARDUINOJSON_USE_8_BYTE_POOL
inline const EightByteValue* VariantImpl::getEightByte() const {
  return type() & VariantTypeBits::EightByteBit
             ? resources_->getEightByte(data_->content.asSlotId)
             : 0;
}
#endif

inline const char* VariantImpl::asLinkedString() const {
  ARDUINOJSON_ASSERT(type() == VariantType::LinkedString);
  return resources_->getStaticString(data_->content.asSlotId);
}

template <typename T>
enable_if_t<sizeof(T) == 8, bool> VariantImpl::setFloat(T value) {
  ARDUINOJSON_ASSERT(isNull());  // must call clear() first

  if (!data_)
    return false;

  float valueAsFloat = static_cast<float>(value);

#if ARDUINOJSON_USE_DOUBLE
  if (value == valueAsFloat) {
    data_->type = VariantType::Float;
    data_->content.asFloat = valueAsFloat;
  } else {
    auto slot = resources_->allocEightByte();
    if (!slot)
      return false;
    data_->type = VariantType::Double;
    data_->content.asSlotId = slot.id();
    slot->asDouble = value;
  }
#else
  data_->type = VariantType::Float;
  data_->content.asFloat = valueAsFloat;
#endif
  return true;
}

template <typename T>
enable_if_t<is_signed<T>::value, bool> VariantImpl::setInteger(T value) {
  ARDUINOJSON_ASSERT(isNull());  // must call clear() first

  if (!data_)
    return false;

  if (canConvertNumber<int32_t>(value)) {
    data_->type = VariantType::Int32;
    data_->content.asInt32 = static_cast<int32_t>(value);
  }
#if ARDUINOJSON_USE_LONG_LONG
  else {
    auto slot = resources_->allocEightByte();
    if (!slot)
      return false;
    data_->type = VariantType::Int64;
    data_->content.asSlotId = slot.id();
    slot->asInt64 = value;
  }
#endif
  return true;
}

template <typename T>
enable_if_t<is_unsigned<T>::value, bool> VariantImpl::setInteger(T value) {
  ARDUINOJSON_ASSERT(isNull());  // must call clear() first

  if (!data_)
    return false;

  if (canConvertNumber<uint32_t>(value)) {
    data_->type = VariantType::Uint32;
    data_->content.asUint32 = static_cast<uint32_t>(value);
  }
#if ARDUINOJSON_USE_LONG_LONG
  else {
    auto slot = resources_->allocEightByte();
    if (!slot)
      return false;
    data_->type = VariantType::Uint64;
    data_->content.asSlotId = slot.id();
    slot->asUint64 = value;
  }
#endif
  return true;
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
