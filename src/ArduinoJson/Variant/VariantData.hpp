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

  JsonString asRawString() const {
    switch (type) {
      case VariantType::RawString:
        return JsonString(content.asStringNode->data,
                          content.asStringNode->length);
      default:
        return JsonString();
    }
  }

  JsonString asString() const {
    switch (type) {
      case VariantType::TinyString:
        return JsonString(content.asTinyString);
      case VariantType::LongString:
        return JsonString(content.asStringNode->data,
                          content.asStringNode->length);
      default:
        return JsonString();
    }
  }

  bool isArray() const {
    return type == VariantType::Array;
  }

  bool isBoolean() const {
    return type == VariantType::Boolean;
  }

  bool isFloat() const {
    return type & VariantTypeBits::NumberBit;
  }

  bool isNull() const {
    return type == VariantType::Null;
  }

  bool isObject() const {
    return type == VariantType::Object;
  }

  bool isString() const {
    return type == VariantType::LongString || type == VariantType::TinyString;
  }

  void setBoolean(bool value) {
    ARDUINOJSON_ASSERT(type == VariantType::Null);
    type = VariantType::Boolean;
    content.asBoolean = value;
  }

  void setRawString(StringNode* s) {
    ARDUINOJSON_ASSERT(type == VariantType::Null);
    ARDUINOJSON_ASSERT(s);
    type = VariantType::RawString;
    content.asStringNode = s;
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

  void setLongString(StringNode* s) {
    ARDUINOJSON_ASSERT(type == VariantType::Null);
    ARDUINOJSON_ASSERT(s);
    type = VariantType::LongString;
    content.asStringNode = s;
  }

  CollectionData* toArray() {
    ARDUINOJSON_ASSERT(type == VariantType::Null);
    type = VariantType::Array;
    return new (&content.asCollection) CollectionData();
  }

  CollectionData* toObject() {
    ARDUINOJSON_ASSERT(type == VariantType::Null);
    type = VariantType::Object;
    return new (&content.asCollection) CollectionData();
  }
};

// HACK: large functions are implemented in static function to give opportunity
// to the compiler to optimize the `this` pointer away.
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
    return accept(visit, data_, resources_);
  }

  template <typename TVisitor>
  static typename TVisitor::result_type accept(TVisitor&, VariantData*,
                                               ResourceManager*);

  VariantData* addElement() {
    return addElement(data_, resources_);
  }

  static VariantData* addElement(VariantData* data,
                                 ResourceManager* resources) {
    if (!data)
      return nullptr;
    auto array = data->type == VariantType::Null ? toArray(data, resources)
                                                 : asArray(data, resources);
    return array.addElement();
  }

  template <typename T>
  bool addValue(const T& value) {
    return addValue(value, data_, resources_);
  }

  template <typename T>
  static bool addValue(const T& value, VariantData* data,
                       ResourceManager* resources) {
    if (!data)
      return false;
    auto array = data->type == VariantType::Null ? toArray(data, resources)
                                                 : asArray(data, resources);
    return array.addValue(value);
  }

  bool asBoolean() const {
    return asBoolean(data_, resources_);
  }

  static bool asBoolean(VariantData* data, ResourceManager* resources) {
    if (!data)
      return false;

#if ARDUINOJSON_USE_8_BYTE_POOL
    auto eightByteValue = getEightByte(data, resources);
#endif
    switch (data->type) {
      case VariantType::Boolean:
        return data->content.asBoolean;
      case VariantType::Uint32:
      case VariantType::Int32:
        return data->content.asUint32 != 0;
      case VariantType::Float:
        return data->content.asFloat != 0;
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
    return asArray(data_, resources_);
  }

  static ArrayImpl asArray(VariantData* data, ResourceManager* resources) {
    return ArrayImpl(data && data->type == VariantType::Array
                         ? &data->content.asCollection
                         : nullptr,
                     resources);
  }

  CollectionImpl asCollection() {
    return CollectionImpl(
        isCollection() ? &data_->content.asCollection : nullptr, resources_);
  }

  template <typename T>
  T asFloat() const {
    return asFloat<T>(data_, resources_);
  }

  template <typename T>
  static T asFloat(VariantData* data, ResourceManager* resources) {
    if (!data)
      return 0.0;

    static_assert(is_floating_point<T>::value, "T must be a floating point");
#if ARDUINOJSON_USE_8_BYTE_POOL
    auto eightByteValue = getEightByte(data, resources);
#endif
    const char* str = nullptr;
    switch (data->type) {
      case VariantType::Boolean:
        return static_cast<T>(data->content.asBoolean);
      case VariantType::Uint32:
        return static_cast<T>(data->content.asUint32);
      case VariantType::Int32:
        return static_cast<T>(data->content.asInt32);
#if ARDUINOJSON_USE_LONG_LONG
      case VariantType::Uint64:
        return static_cast<T>(eightByteValue->asUint64);
      case VariantType::Int64:
        return static_cast<T>(eightByteValue->asInt64);
#endif
      case VariantType::TinyString:
        str = data->content.asTinyString;
        break;
      case VariantType::LongString:
        str = data->content.asStringNode->data;
        break;
      case VariantType::Float:
        return static_cast<T>(data->content.asFloat);
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
    return asIntegral<T>(data_, resources_);
  }

  template <typename T>
  static T asIntegral(VariantData* data, ResourceManager* resources) {
    if (!data)
      return 0;

    static_assert(is_integral<T>::value, "T must be an integral type");
#if ARDUINOJSON_USE_8_BYTE_POOL
    auto eightByteValue = getEightByte(data, resources);
#endif
    const char* str = nullptr;
    switch (data->type) {
      case VariantType::Boolean:
        return data->content.asBoolean;
      case VariantType::Uint32:
        return convertNumber<T>(data->content.asUint32);
      case VariantType::Int32:
        return convertNumber<T>(data->content.asInt32);
#if ARDUINOJSON_USE_LONG_LONG
      case VariantType::Uint64:
        return convertNumber<T>(eightByteValue->asUint64);
      case VariantType::Int64:
        return convertNumber<T>(eightByteValue->asInt64);
#endif
      case VariantType::TinyString:
        str = data->content.asTinyString;
        break;
      case VariantType::LongString:
        str = data->content.asStringNode->data;
        break;
      case VariantType::Float:
        return convertNumber<T>(data->content.asFloat);
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
    return asObject(data_, resources_);
  }

  static ObjectImpl asObject(VariantData* data, ResourceManager* resources) {
    return ObjectImpl(data && data->type == VariantType::Object
                          ? &data->content.asCollection
                          : nullptr,
                      resources);
  }

#if ARDUINOJSON_USE_8_BYTE_POOL
  static const EightByteValue* getEightByte(VariantData*, ResourceManager*);
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
    return getOrAddMember(key, data_, resources_);
  }

  template <typename TAdaptedString>
  static VariantData* getOrAddMember(TAdaptedString key, VariantData* data,
                                     ResourceManager* resources) {
    if (key.isNull())
      return nullptr;
    if (!data)
      return nullptr;
    auto obj = data->type == VariantType::Null ? toObject(data, resources)
                                               : asObject(data, resources);
    return obj.getOrAddMember(key);
  }

  bool isArray() const {
    return type() == VariantType::Array;
  }

  bool isCollection() const {
    return type() & VariantTypeBits::CollectionMask;
  }

  template <typename T>
  bool isInteger() const {
    return isInteger<T>(data_, resources_);
  }

  template <typename T>
  static bool isInteger(VariantData* data, ResourceManager* resources) {
    if (!data)
      return false;

#if ARDUINOJSON_USE_LONG_LONG
    auto eightByteValue = getEightByte(data, resources);
#else
    (void)resources;
#endif
    switch (data->type) {
      case VariantType::Uint32:
        return canConvertNumber<T>(data->content.asUint32);

      case VariantType::Int32:
        return canConvertNumber<T>(data->content.asInt32);

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
    clear(data_, resources_);
    data_->setBoolean(value);
    return true;
  }

  template <typename T>
  bool setFloat(T value) {
    if (!data_)
      return false;
    clear(data_, resources_);
    return setFloat(value, data_, resources_);
  }

  template <typename T>
  static enable_if_t<sizeof(T) == 4, bool> setFloat(T value, VariantData* data,
                                                    ResourceManager*) {
    ARDUINOJSON_ASSERT(data != nullptr);
    ARDUINOJSON_ASSERT(data->type == VariantType::Null);
    data->type = VariantType::Float;
    data->content.asFloat = value;
    return true;
  }

  template <typename T>
  static enable_if_t<sizeof(T) == 8, bool> setFloat(T value, VariantData*,
                                                    ResourceManager*);

  template <typename T>
  bool setInteger(T value) {
    if (!data_)
      return false;
    clear(data_, resources_);
    return setInteger(value, data_, resources_);
  }

  template <typename T>
  static enable_if_t<is_signed<T>::value, bool> setInteger(T value,
                                                           VariantData*,
                                                           ResourceManager*);

  template <typename T>
  static enable_if_t<is_unsigned<T>::value, bool> setInteger(T value,
                                                             VariantData*,
                                                             ResourceManager*);

  template <typename T>
  void setRawString(SerializedValue<T> value);

  template <typename TAdaptedString>
  bool setString(TAdaptedString value) {
    if (!data_)
      return false;
    clear(data_, resources_);
    return setString(value, data_, resources_);
  }

  template <typename TAdaptedString>
  static bool setString(TAdaptedString value, VariantData*, ResourceManager*);

  size_t size() {
    if (isObject())
      return asObject().size();

    if (isArray())
      return asArray().size();

    return 0;
  }

  ArrayImpl toArray() {
    if (!data_)
      return ArrayImpl();
    clear(data_, resources_);
    return toArray(data_, resources_);
  }

  static ArrayImpl toArray(VariantData* data, ResourceManager* resources) {
    ARDUINOJSON_ASSERT(data != nullptr);
    ARDUINOJSON_ASSERT(resources != nullptr);
    return ArrayImpl(data->toArray(), resources);
  }

  ObjectImpl toObject() {
    if (!data_)
      return ObjectImpl();
    clear(data_, resources_);
    return toObject(data_, resources_);
  }

  static ObjectImpl toObject(VariantData* data, ResourceManager* resources) {
    ARDUINOJSON_ASSERT(data != nullptr);
    ARDUINOJSON_ASSERT(resources != nullptr);
    return ObjectImpl(data->toObject(), resources);
  }

  VariantType type() const {
    return data_ ? data_->type : VariantType::Null;
  }

  // Release the resources used by this variant and set it to null.
  void clear() {
    if (data_)
      clear(data_, resources_);
  }

  static void clear(VariantData*, ResourceManager*);

 private:
  VariantData* data_;
  ResourceManager* resources_;
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
