// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Memory/MemoryPool.hpp>
#include <ArduinoJson/Memory/StringNode.hpp>
#include <ArduinoJson/Misc/SerializedValue.hpp>
#include <ArduinoJson/Numbers/convertNumber.hpp>
#include <ArduinoJson/Strings/JsonString.hpp>
#include <ArduinoJson/Strings/StringAdapters.hpp>
#include <ArduinoJson/Variant/VariantContent.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename T>
T parseNumber(const char* s);

template <typename T>
static bool isTinyString(const T& s, size_t n) {
  if (n > tinyStringMaxLength)
    return false;
  bool containsNul = false;
  for (uint8_t i = 0; i < uint8_t(n); i++)
    containsNul |= !s[i];
  return !containsNul;
}

struct VariantData {
  VariantContent content;  // must be first to allow cast from array to variant
  VariantType type = VariantType::Null;
  SlotId next = NULL_SLOT;

  // Placement new
  static void* operator new(size_t, void* p) noexcept {
    return p;
  }

  static void operator delete(void*, void*) noexcept {}

  void setBoolean(bool value) {
    ARDUINOJSON_ASSERT(type == VariantType::Null);
    type = VariantType::Boolean;
    content.asBoolean = value;
  }

  template <typename TAdaptedString>
  void setTinyString(const TAdaptedString& s) {
    ARDUINOJSON_ASSERT(type == VariantType::Null);
    ARDUINOJSON_ASSERT(s.size() <= tinyStringMaxLength);

    type = VariantType::TinyString;

    auto n = uint8_t(s.size());
    for (uint8_t i = 0; i < n; i++) {
      char c = s[i];
      ARDUINOJSON_ASSERT(c != 0);  // no NUL in tiny string
      content.asTinyString[i] = c;
    }

    content.asTinyString[n] = 0;
  }

  void setOwnedString(StringNode* s) {
    ARDUINOJSON_ASSERT(type == VariantType::Null);
    ARDUINOJSON_ASSERT(s);
    type = VariantType::OwnedString;
    content.asOwnedString = s;
  }

  void setRawString(StringNode* s) {
    ARDUINOJSON_ASSERT(type == VariantType::Null);
    ARDUINOJSON_ASSERT(s);
    type = VariantType::RawString;
    content.asOwnedString = s;
  }

  bool isFloat() const {
    return type & VariantTypeBits::NumberBit;
  }

  bool isString() const {
    return type == VariantType::LinkedString ||
           type == VariantType::OwnedString || type == VariantType::TinyString;
  }
};

class VariantImpl {
 public:
  VariantImpl() : data_(nullptr), resources_(nullptr) {}

  VariantImpl(VariantData* data, ResourceManager* resources)
      : data_(data), resources_(resources) {}

  VariantData* getData() const {
    return data_;
  }

  ResourceManager* getResourceManager() const {
    return resources_;
  }

  template <typename TVisitor>
  typename TVisitor::result_type accept(TVisitor& visit) {
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
        return visit.visit(asArray());

      case VariantType::Object:
        return visit.visit(asObject());

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

  VariantData* addElement() {
    auto array = isNull() ? toArray() : asArray();
    return array.addElement();
  }

  template <typename T>
  bool addValue(const T& value) {
    auto array = isNull() ? toArray() : asArray();
    return array.addValue(value);
  }

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

  ArrayImpl asArray() {
    return ArrayImpl(isArray() ? &data_->content.asCollection : nullptr,
                     resources_);
  }

  CollectionImpl asCollection() {
    return CollectionImpl(
        isCollection() ? &data_->content.asCollection : nullptr, resources_);
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

  ObjectImpl asObject() {
    return ObjectImpl(isObject() ? &data_->content.asCollection : nullptr,
                      resources_);
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

  const char* asLinkedString() const;

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
  const EightByteValue* getEightByte() const;
#endif

  VariantData* getElement(size_t index) {
    return asArray().getElement(index);
  }

  template <typename TAdaptedString>
  VariantData* getMember(TAdaptedString key) {
    return asObject().getMember(key);
  }

  VariantData* getOrAddElement(size_t index) {
    auto array = isNull() ? toArray() : asArray();
    return array.getOrAddElement(index);
  }

  template <typename TAdaptedString>
  VariantData* getOrAddMember(TAdaptedString key) {
    if (key.isNull())
      return nullptr;
    auto obj = isNull() ? toObject() : asObject();
    return obj.getOrAddMember(key);
  }

  bool isArray() const {
    return type() == VariantType::Array;
  }

  bool isBoolean() const {
    return type() == VariantType::Boolean;
  }

  bool isCollection() const {
    return type() & VariantTypeBits::CollectionMask;
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

  bool isNull() const {
    return type() == VariantType::Null;
  }

  bool isObject() const {
    return type() == VariantType::Object;
  }

  bool isString() const {
    return data_ && data_->isString();
  }

  size_t nesting() {
    return asCollection().nesting();
  }

  void removeElement(size_t index) {
    asArray().removeElement(index);
  }

  template <typename TAdaptedString>
  void removeMember(TAdaptedString key) {
    asObject().removeMember(key);
  }

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
  enable_if_t<sizeof(T) == 8, bool> setFloat(T value);

  template <typename T>
  enable_if_t<is_signed<T>::value, bool> setInteger(T value);

  template <typename T>
  enable_if_t<is_unsigned<T>::value, bool> setInteger(T value);

  template <typename T>
  void setRawString(SerializedValue<T> value);

  template <typename TAdaptedString>
  bool setString(TAdaptedString value);

  bool setLinkedString(const char* s);

  size_t size() {
    if (isObject())
      return asObject().size();

    if (isArray())
      return asArray().size();

    return 0;
  }

  ArrayImpl toArray() {
    ARDUINOJSON_ASSERT(type() == VariantType::Null);  // must call clear() first
    if (!data_)
      return ArrayImpl();
    data_->type = VariantType::Array;
    return ArrayImpl(new (&data_->content.asCollection) CollectionData(),
                     resources_);
  }

  ObjectImpl toObject() {
    ARDUINOJSON_ASSERT(type() == VariantType::Null);  // must call clear() first
    if (!data_)
      return ObjectImpl();
    data_->type = VariantType::Object;
    return ObjectImpl(new (&data_->content.asCollection) CollectionData(),
                      resources_);
  }

  VariantType type() const {
    return data_ ? data_->type : VariantType::Null;
  }

  // Release the resources used by this variant and set it to null.
  void clear();

 private:
  VariantData* data_;
  ResourceManager* resources_;
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
