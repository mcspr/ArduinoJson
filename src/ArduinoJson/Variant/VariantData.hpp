// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Memory/StringNode.hpp>
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

  CollectionData* asCollection() {
    return type & VariantTypeBits::CollectionMask ? &content.asCollection
                                                  : nullptr;
  }

  CollectionData* asArray() {
    return type == VariantType::Array ? &content.asCollection : nullptr;
  }

  CollectionData* asObject() {
    return type == VariantType::Object ? &content.asCollection : nullptr;
  }

  bool isFloat() const {
    return type & VariantTypeBits::NumberBit;
  }

  bool isString() const {
    return type == VariantType::LinkedString ||
           type == VariantType::OwnedString || type == VariantType::TinyString;
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

  CollectionData* getOrCreateArray() {
    switch (type) {
      case VariantType::Null:
        return toArray();
      case VariantType::Array:
        return &content.asCollection;
      default:
        return nullptr;
    }
  }

  CollectionData* getOrCreateObject() {
    switch (type) {
      case VariantType::Null:
        return toObject();
      case VariantType::Object:
        return &content.asCollection;
      default:
        return nullptr;
    }
  }
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
