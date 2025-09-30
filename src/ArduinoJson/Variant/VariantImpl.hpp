// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Memory/ResourceManager.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename TVisitor>
typename TVisitor::result_type VariantImpl::accept(TVisitor& visit,
                                                   VariantData* data,
                                                   ResourceManager* resources) {
  if (!data)
    return visit.visit(nullptr);

#if ARDUINOJSON_USE_8_BYTE_POOL
  auto eightByteValue = getEightByte(data, resources);
#endif
  switch (data->type) {
    case VariantType::Float:
      return visit.visit(data->content.asFloat);

#if ARDUINOJSON_USE_DOUBLE
    case VariantType::Double:
      return visit.visit(eightByteValue->asDouble);
#endif

    case VariantType::Array:
      return visit.visit(ArrayImpl(&data->content.asCollection, resources));

    case VariantType::Object:
      return visit.visit(ObjectImpl(&data->content.asCollection, resources));

    case VariantType::TinyString:
      return visit.visit(JsonString(data->content.asTinyString));

    case VariantType::LongString:
      return visit.visit(JsonString(data->content.asStringNode->data,
                                    data->content.asStringNode->length));

    case VariantType::RawString:
      return visit.visit(RawString(data->content.asStringNode->data,
                                   data->content.asStringNode->length));

    case VariantType::Int32:
      return visit.visit(static_cast<JsonInteger>(data->content.asInt32));

    case VariantType::Uint32:
      return visit.visit(static_cast<JsonUInt>(data->content.asUint32));

#if ARDUINOJSON_USE_LONG_LONG
    case VariantType::Int64:
      return visit.visit(eightByteValue->asInt64);

    case VariantType::Uint64:
      return visit.visit(eightByteValue->asUint64);
#endif

    case VariantType::Boolean:
      return visit.visit(data->content.asBoolean != 0);

    default:
      return visit.visit(nullptr);
  }
}

template <typename T>
inline void VariantImpl::setRawString(SerializedValue<T> value) {
  if (!data_)
    return;
  clear(data_, resources_);
  auto dup = resources_->saveString(adaptString(value.data(), value.size()));
  if (dup)
    data_->setRawString(dup);
}

template <typename TAdaptedString>
inline bool VariantImpl::setString(TAdaptedString value, VariantData* data,
                                   ResourceManager* resources) {
  ARDUINOJSON_ASSERT(data != nullptr);
  ARDUINOJSON_ASSERT(data->type == VariantType::Null);
  ARDUINOJSON_ASSERT(resources != nullptr);

  if (value.isNull())
    return false;

  if (isTinyString(value, value.size())) {
    data->setTinyString(value);
    return true;
  }

  auto dup = resources->saveString(value);
  if (dup) {
    data->setLongString(dup);
    return true;
  }

  return false;
}

inline void VariantImpl::clear(VariantData* data, ResourceManager* resources) {
  ARDUINOJSON_ASSERT(data != nullptr);
  ARDUINOJSON_ASSERT(resources != nullptr);

  if (data->type & VariantTypeBits::OwnedStringBit)
    resources->dereferenceString(data->content.asStringNode->data);

#if ARDUINOJSON_USE_8_BYTE_POOL
  if (data->type & VariantTypeBits::EightByteBit)
    resources->freeEightByte(data->content.asSlotId);
#endif

  if (data->type & VariantTypeBits::CollectionMask)
    CollectionImpl(&data->content.asCollection, resources).clear();

  data->type = VariantType::Null;
}

#if ARDUINOJSON_USE_8_BYTE_POOL
inline const EightByteValue* VariantImpl::getEightByte(
    VariantData* data, ResourceManager* resources) {
  ARDUINOJSON_ASSERT(data != nullptr);
  ARDUINOJSON_ASSERT(resources != nullptr);
  return data->type & VariantTypeBits::EightByteBit
             ? resources->getEightByte(data->content.asSlotId)
             : 0;
}

#endif

template <typename T>
enable_if_t<sizeof(T) == 8, bool> VariantImpl::setFloat(
    T value, VariantData* data, ResourceManager* resources) {
  ARDUINOJSON_ASSERT(data != nullptr);
  ARDUINOJSON_ASSERT(data->type == VariantType::Null);
  ARDUINOJSON_ASSERT(resources != nullptr);

  float valueAsFloat = static_cast<float>(value);

#if ARDUINOJSON_USE_DOUBLE
  if (value == valueAsFloat) {
    data->type = VariantType::Float;
    data->content.asFloat = valueAsFloat;
  } else {
    auto slot = resources->allocEightByte();
    if (!slot)
      return false;
    data->type = VariantType::Double;
    data->content.asSlotId = slot.id();
    slot->asDouble = value;
  }
#else
  data->type = VariantType::Float;
  data->content.asFloat = valueAsFloat;
#endif
  return true;
}

template <typename T>
enable_if_t<is_signed<T>::value, bool> VariantImpl::setInteger(
    T value, VariantData* data, ResourceManager* resources) {
  ARDUINOJSON_ASSERT(data != nullptr);
  ARDUINOJSON_ASSERT(data->type == VariantType::Null);
  ARDUINOJSON_ASSERT(resources != nullptr);

  if (canConvertNumber<int32_t>(value)) {
    data->type = VariantType::Int32;
    data->content.asInt32 = static_cast<int32_t>(value);
  }
#if ARDUINOJSON_USE_LONG_LONG
  else {
    auto slot = resources->allocEightByte();
    if (!slot)
      return false;
    data->type = VariantType::Int64;
    data->content.asSlotId = slot.id();
    slot->asInt64 = value;
  }
#else
  (void)resources;
#endif
  return true;
}

template <typename T>
enable_if_t<is_unsigned<T>::value, bool> VariantImpl::setInteger(
    T value, VariantData* data, ResourceManager* resources) {
  ARDUINOJSON_ASSERT(data != nullptr);
  ARDUINOJSON_ASSERT(data->type == VariantType::Null);
  ARDUINOJSON_ASSERT(resources != nullptr);

  if (canConvertNumber<uint32_t>(value)) {
    data->type = VariantType::Uint32;
    data->content.asUint32 = static_cast<uint32_t>(value);
  }
#if ARDUINOJSON_USE_LONG_LONG
  else {
    auto slot = resources->allocEightByte();
    if (!slot)
      return false;
    data->type = VariantType::Uint64;
    data->content.asSlotId = slot.id();
    slot->asUint64 = value;
  }
#else
  (void)resources;
#endif
  return true;
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
