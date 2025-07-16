// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Memory/Allocator.hpp>
#include <ArduinoJson/Memory/MemoryPoolList.hpp>
#include <ArduinoJson/Memory/StringPool.hpp>
#include <ArduinoJson/Polyfills/assert.hpp>
#include <ArduinoJson/Polyfills/utility.hpp>
#include <ArduinoJson/Strings/StringAdapters.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

struct VariantData;
class VariantWithId;

class ResourceManager {
 public:
  ResourceManager(Allocator* allocator = DefaultAllocator::instance())
      : allocator_(allocator), overflowed_(false) {}

  ~ResourceManager() {
    stringPool_.clear(allocator_);
    variantPools_.clear(allocator_);
    staticStringsPools_.clear(allocator_);
#if ARDUINOJSON_USE_8_BYTE_POOL
    eightBytePools_.clear(allocator_);
#endif
  }

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager& src) = delete;

  friend void swap(ResourceManager& a, ResourceManager& b) {
    swap(a.stringPool_, b.stringPool_);
    swap(a.variantPools_, b.variantPools_);
    swap(a.staticStringsPools_, b.staticStringsPools_);
#if ARDUINOJSON_USE_8_BYTE_POOL
    swap(a.eightBytePools_, b.eightBytePools_);
#endif
    swap_(a.allocator_, b.allocator_);
    swap_(a.overflowed_, b.overflowed_);
  }

  Allocator* allocator() const {
    return allocator_;
  }

  size_t size() const {
    return variantPools_.size() + stringPool_.size();
  }

  bool overflowed() const {
    return overflowed_;
  }

  Slot<VariantData> allocVariant() {
    auto slot = variantPools_.allocSlot(allocator_);
    if (!slot) {
      overflowed_ = true;
      return {};
    }
    new (slot.ptr()) VariantData();
    return slot;
  }

  void freeVariant(Slot<VariantData> slot) {
    variantPools_.freeSlot(slot);
  }

  VariantData* getVariant(SlotId id) const {
    return variantPools_.getSlot(id);
  }

#if ARDUINOJSON_USE_8_BYTE_POOL
  Slot<EightByteValue> allocEightByte() {
    auto slot = eightBytePools_.allocSlot(allocator_);
    if (!slot) {
      overflowed_ = true;
      return {};
    }
    return slot;
  }

  void freeEightByte(SlotId id) {
    auto p = getEightByte(id);
    eightBytePools_.freeSlot({p, id});
  }

  EightByteValue* getEightByte(SlotId id) const {
    return eightBytePools_.getSlot(id);
  }
#endif

  template <typename TAdaptedString>
  StringNode* saveString(TAdaptedString str) {
    if (str.isNull())
      return 0;

    auto node = stringPool_.add(str, allocator_);
    if (!node)
      overflowed_ = true;

    return node;
  }

  void saveString(StringNode* node) {
    stringPool_.add(node);
  }

  template <typename TAdaptedString>
  StringNode* getString(const TAdaptedString& str) const {
    return stringPool_.get(str);
  }

  StringNode* createString(size_t length) {
    auto node = StringNode::create(length, allocator_);
    if (!node)
      overflowed_ = true;
    return node;
  }

  StringNode* resizeString(StringNode* node, size_t length) {
    node = StringNode::resize(node, length, allocator_);
    if (!node)
      overflowed_ = true;
    return node;
  }

  void destroyString(StringNode* node) {
    StringNode::destroy(node, allocator_);
  }

  void dereferenceString(StringNode* node) {
    stringPool_.dereference(node, allocator_);
  }

  SlotId saveStaticString(const char* s) {
    auto existingSlotId = staticStringsPools_.find(s);
    if (existingSlotId != NULL_SLOT)
      return existingSlotId;

    auto slot = staticStringsPools_.allocSlot(allocator_);
    if (slot)
      *slot = s;
    else
      overflowed_ = true;

    return slot.id();
  }

  const char* getStaticString(SlotId id) const {
    return *staticStringsPools_.getSlot(id);
  }

  void clear() {
    overflowed_ = false;
    variantPools_.clear(allocator_);
    stringPool_.clear(allocator_);
    staticStringsPools_.clear(allocator_);
#if ARDUINOJSON_USE_8_BYTE_POOL
    eightBytePools_.clear(allocator_);
#endif
  }

  void shrinkToFit() {
    variantPools_.shrinkToFit(allocator_);
    staticStringsPools_.shrinkToFit(allocator_);
#if ARDUINOJSON_USE_8_BYTE_POOL
    eightBytePools_.shrinkToFit(allocator_);
#endif
  }

 private:
  Allocator* allocator_;
  bool overflowed_;
  StringPool stringPool_;
  MemoryPoolList<VariantData> variantPools_;
  MemoryPoolList<const char*> staticStringsPools_;
#if ARDUINOJSON_USE_8_BYTE_POOL
  MemoryPoolList<EightByteValue> eightBytePools_;
#endif
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
