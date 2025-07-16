// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Memory/ResourceManager.hpp>
#include <ArduinoJson/Misc/SerializedValue.hpp>
#include <ArduinoJson/Numbers/convertNumber.hpp>
#include <ArduinoJson/Strings/JsonString.hpp>
#include <ArduinoJson/Strings/StringAdapters.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

class CollectionIterator;

class VariantImpl {
  VariantData* data_;
  ResourceManager* resources_;

 public:
  using iterator = CollectionIterator;

  VariantImpl() : data_(nullptr), resources_(nullptr) {}

  VariantImpl(VariantData* data, ResourceManager* resources)
      : data_(data), resources_(resources) {}

  VariantData* data() const {
    return data_;
  }

  ResourceManager* resources() const {
    return resources_;
  }

  template <typename TVisitor>
  typename TVisitor::result_type accept(TVisitor& visit) const {
    if (!data_)
      return visit.visit(nullptr);

#if ARDUINOJSON_USE_8_BYTE_POOL
    auto eightByteValue = getEightByte();
#endif
    switch (data_->type) {
      case VariantType::Float:
        return visit.visit(data_->content.asFloat);

#if ARDUINOJSON_USE_DOUBLE
      case VariantType::Double:
        return visit.visit(eightByteValue->asDouble);
#endif

      case VariantType::Array:
        return visit.visitArray(VariantImpl(data_, resources_));

      case VariantType::Object:
        return visit.visitObject(VariantImpl(data_, resources_));

      case VariantType::TinyString:
        return visit.visit(JsonString(data_->content.asTinyString));

      case VariantType::LinkedString:
        return visit.visit(JsonString(asLinkedString(), true));

      case VariantType::OwnedString:
        return visit.visit(JsonString(data_->content.asOwnedString->data,
                                      data_->content.asOwnedString->length));

      case VariantType::RawString:
        return visit.visit(RawString(data_->content.asOwnedString->data,
                                     data_->content.asOwnedString->length));

      case VariantType::Int32:
        return visit.visit(static_cast<JsonInteger>(data_->content.asInt32));

      case VariantType::Uint32:
        return visit.visit(static_cast<JsonUInt>(data_->content.asUint32));

#if ARDUINOJSON_USE_LONG_LONG
      case VariantType::Int64:
        return visit.visit(eightByteValue->asInt64);

      case VariantType::Uint64:
        return visit.visit(eightByteValue->asUint64);
#endif

      case VariantType::Boolean:
        return visit.visit(data_->content.asBoolean != 0);

      default:
        return visit.visit(nullptr);
    }
  }

  VariantData* addElement();

  template <typename T>
  bool addValue(const T& value);

  bool asBoolean() const {
    if (!data_)
      return false;

#if ARDUINOJSON_USE_8_BYTE_POOL
    auto eightByteValue = getEightByte();
#endif
    switch (data_->type) {
      case VariantType::Boolean:
        return data_->content.asBoolean;
      case VariantType::Uint32:
      case VariantType::Int32:
        return data_->content.asUint32 != 0;
      case VariantType::Float:
        return data_->content.asFloat != 0;
#if ARDUINOJSON_USE_DOUBLE
      case VariantType::Double:
        return eightByteValue->asDouble != 0;
#endif
      case VariantType::Null:
        return false;
#if ARDUINOJSON_USE_LONG_LONG
      case VariantType::Uint64:
      case VariantType::Int64:
        return eightByteValue->asUint64 != 0;
#endif
      default:
        return true;
    }
  }

  template <typename T>
  T asFloat() const {
    if (!data_)
      return 0.0;

    static_assert(is_floating_point<T>::value, "T must be a floating point");
#if ARDUINOJSON_USE_8_BYTE_POOL
    auto eightByteValue = getEightByte();
#endif
    const char* str = nullptr;
    switch (data_->type) {
      case VariantType::Boolean:
        return static_cast<T>(data_->content.asBoolean);
      case VariantType::Uint32:
        return static_cast<T>(data_->content.asUint32);
      case VariantType::Int32:
        return static_cast<T>(data_->content.asInt32);
#if ARDUINOJSON_USE_LONG_LONG
      case VariantType::Uint64:
        return static_cast<T>(eightByteValue->asUint64);
      case VariantType::Int64:
        return static_cast<T>(eightByteValue->asInt64);
#endif
      case VariantType::TinyString:
        str = data_->content.asTinyString;
        break;
      case VariantType::LinkedString:
        str = asLinkedString();
        break;
      case VariantType::OwnedString:
        str = data_->content.asOwnedString->data;
        break;
      case VariantType::Float:
        return static_cast<T>(data_->content.asFloat);
#if ARDUINOJSON_USE_DOUBLE
      case VariantType::Double:
        return static_cast<T>(eightByteValue->asDouble);
#endif
      default:
        return 0.0;
    }

    ARDUINOJSON_ASSERT(str != nullptr);
    return parseNumber<T>(str);
  }

  template <typename T>
  T asIntegral() const {
    if (!data_)
      return 0;

    static_assert(is_integral<T>::value, "T must be an integral type");
#if ARDUINOJSON_USE_8_BYTE_POOL
    auto eightByteValue = getEightByte();
#endif
    const char* str = nullptr;
    switch (data_->type) {
      case VariantType::Boolean:
        return data_->content.asBoolean;
      case VariantType::Uint32:
        return convertNumber<T>(data_->content.asUint32);
      case VariantType::Int32:
        return convertNumber<T>(data_->content.asInt32);
#if ARDUINOJSON_USE_LONG_LONG
      case VariantType::Uint64:
        return convertNumber<T>(eightByteValue->asUint64);
      case VariantType::Int64:
        return convertNumber<T>(eightByteValue->asInt64);
#endif
      case VariantType::TinyString:
        str = data_->content.asTinyString;
        break;
      case VariantType::LinkedString:
        str = asLinkedString();
        break;
      case VariantType::OwnedString:
        str = data_->content.asOwnedString->data;
        break;
      case VariantType::Float:
        return convertNumber<T>(data_->content.asFloat);
#if ARDUINOJSON_USE_DOUBLE
      case VariantType::Double:
        return convertNumber<T>(eightByteValue->asDouble);
#endif
      default:
        return 0;
    }

    ARDUINOJSON_ASSERT(str != nullptr);
    return parseNumber<T>(str);
  }

  JsonString asRawString() const {
    switch (type()) {
      case VariantType::RawString:
        return JsonString(data_->content.asOwnedString->data,
                          data_->content.asOwnedString->length);
      default:
        return JsonString();
    }
  }

  const char* asLinkedString() const {
    ARDUINOJSON_ASSERT(type() == VariantType::LinkedString);
    return resources_->getStaticString(data_->content.asSlotId);
  }

  JsonString asString() const {
    switch (type()) {
      case VariantType::TinyString:
        return JsonString(data_->content.asTinyString);
      case VariantType::LinkedString:
        return JsonString(asLinkedString(), true);
      case VariantType::OwnedString:
        return JsonString(data_->content.asOwnedString->data,
                          data_->content.asOwnedString->length);
      default:
        return JsonString();
    }
  }

#if ARDUINOJSON_USE_8_BYTE_POOL
  const EightByteValue* getEightByte() const {
    return type() & VariantTypeBits::EightByteBit
               ? resources_->getEightByte(data_->content.asSlotId)
               : 0;
  }
#endif

  SlotId head() const {
    return getCollectionData()->head;
  }

  iterator createIterator() const;

  VariantData* getElement(size_t index) const;

  VariantData* getOrAddElement(size_t index);

  VariantData* addPair(VariantData** value);

  template <typename TAdaptedString>
  VariantData* addMember(TAdaptedString key);

  template <typename TAdaptedString>
  VariantData* getMember(TAdaptedString key) const;

  template <typename TAdaptedString>
  VariantData* getOrAddMember(TAdaptedString key);

  bool isArray() const {
    return type() == VariantType::Array;
  }

  bool isBoolean() const {
    return type() == VariantType::Boolean;
  }

  bool isFloat() const {
    return data_ && data_->isFloat();
  }

  template <typename T>
  bool isInteger() const {
    if (!data_)
      return false;

#if ARDUINOJSON_USE_LONG_LONG
    auto eightByteValue = getEightByte();
#endif
    switch (data_->type) {
      case VariantType::Uint32:
        return canConvertNumber<T>(data_->content.asUint32);

      case VariantType::Int32:
        return canConvertNumber<T>(data_->content.asInt32);

#if ARDUINOJSON_USE_LONG_LONG
      case VariantType::Uint64:
        return canConvertNumber<T>(eightByteValue->asUint64);

      case VariantType::Int64:
        return canConvertNumber<T>(eightByteValue->asInt64);
#endif

      default:
        return false;
    }
  }

  bool isUnbound() const {
    return !data_;
  }

  bool isNull() const {
    return type() == VariantType::Null;
  }

  bool isObject() const {
    return type() == VariantType::Object;
  }

  bool isString() const {
    return data_ && data_->isString();
  }

  size_t nesting() const;

  void removeElement(iterator it);

  void removeElement(size_t index);

  template <typename TAdaptedString>
  void removeMember(TAdaptedString key);

  void removeMember(iterator it);

  bool setBoolean(bool value) {
    if (!data_)
      return false;
    data_->setBoolean(value);
    return true;
  }

  template <typename T>
  enable_if_t<sizeof(T) == 4, bool> setFloat(T value) {
    ARDUINOJSON_ASSERT(type() == VariantType::Null);  // must call clear() first
    if (!data_)
      return false;
    data_->type = VariantType::Float;
    data_->content.asFloat = value;
    return true;
  }

  template <typename T>
  enable_if_t<sizeof(T) == 8, bool> setFloat(T value) {
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
  enable_if_t<is_signed<T>::value, bool> setInteger(T value) {
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
  enable_if_t<is_unsigned<T>::value, bool> setInteger(T value) {
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

  template <typename TAdaptedString>
  bool setRawString(TAdaptedString value) {
    if (!data_)
      return false;
    auto dup = resources_->saveString(value);
    if (!dup)
      return false;
    data_->setRawString(dup);
    return true;
  }

  template <typename TAdaptedString>
  bool setString(TAdaptedString value) {
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

    return setOwnedString(value);
  }

  template <typename TAdaptedString>
  bool setOwnedString(TAdaptedString value) {
    auto dup = resources_->saveString(value);
    if (!dup)
      return false;

    data_->setOwnedString(dup);
    return true;
  }

  bool setLinkedString(const char* s) {
    ARDUINOJSON_ASSERT(isNull());  // must call clear() first
    ARDUINOJSON_ASSERT(s);

    auto slotId = resources_->saveStaticString(s);
    if (slotId == NULL_SLOT)
      return false;

    data_->type = VariantType::LinkedString;
    data_->content.asSlotId = slotId;
    return true;
  }

  void empty() {
    auto coll = getCollectionData();

    auto next = coll->head;
    while (next != NULL_SLOT) {
      auto currId = next;
      auto slot = getVariant(next);
      next = slot->next;
      freeVariant({slot, currId});
    }

    coll->head = NULL_SLOT;
    coll->tail = NULL_SLOT;
  }

  size_t size() const;

  VariantType type() const {
    return data_ ? data_->type : VariantType::Null;
  }

  // Release the resources used by this variant and set it to null.
  void clear() {
    if (!data_)
      return;

    if (data_->type & VariantTypeBits::OwnedStringBit)
      resources_->dereferenceString(data_->content.asOwnedString->data);

#if ARDUINOJSON_USE_8_BYTE_POOL
    if (data_->type & VariantTypeBits::EightByteBit)
      resources_->freeEightByte(data_->content.asSlotId);
#endif

    if (data_->type & VariantTypeBits::CollectionMask)
      empty();

    data_->type = VariantType::Null;
  }

 private:
  template <typename TAdaptedString>
  iterator findKey(TAdaptedString key) const;

  iterator at(size_t index) const;

  void appendOne(Slot<VariantData> slot);
  void appendPair(Slot<VariantData> key, Slot<VariantData> value);

  void removeOne(iterator it);
  void removePair(iterator it);

  VariantData* getVariant(SlotId id) const {
    ARDUINOJSON_ASSERT(resources_ != nullptr);
    return resources_->getVariant(id);
  }

  void freeVariant(Slot<VariantData> slot) {
    ARDUINOJSON_ASSERT(resources_ != nullptr);
    VariantImpl(slot.ptr(), resources_).clear();
    resources_->freeVariant(slot);
  }

  Slot<VariantData> allocVariant() {
    ARDUINOJSON_ASSERT(resources_ != nullptr);
    return resources_->allocVariant();
  }

  Slot<VariantData> getPreviousSlot(VariantData*) const;

  CollectionData* getCollectionData() const {
    ARDUINOJSON_ASSERT(data_ != nullptr);
    ARDUINOJSON_ASSERT(data_->isCollection());
    return &data_->content.asCollection;
  }
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
