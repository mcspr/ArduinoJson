// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Collection/CollectionData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

class VariantImpl;

class ObjectImpl : public CollectionImpl {
 public:
  ObjectImpl() {}

  ObjectImpl(VariantData* data, ResourceManager* resources)
      : CollectionImpl(data, resources) {}

  bool isNull() const {
    return !data_ || data_->type != VariantType::Object;
  }

  template <typename TAdaptedString>
  VariantData* addMember(TAdaptedString key);

  VariantData* addPair(VariantData** value) {
    if (isNull())
      return nullptr;
    return addPair(value, data_, resources_);
  }

  static VariantData* addPair(VariantData** value, VariantData*,
                              ResourceManager*);

  template <typename TAdaptedString>
  VariantData* getOrAddMember(TAdaptedString key);

  template <typename TAdaptedString>
  VariantData* getMember(TAdaptedString key) const {
    if (isNull())
      return nullptr;
    return getMember(key, data_, resources_);
  }

  template <typename TAdaptedString>
  static VariantData* getMember(TAdaptedString key, VariantData*,
                                ResourceManager*);

  template <typename TAdaptedString>
  void removeMember(TAdaptedString key);

  void remove(iterator it) {
    CollectionImpl::removePair(it);
  }

  size_t size() const {
    return CollectionImpl::size() / 2;
  }

 private:
  template <typename TAdaptedString>
  iterator findKey(TAdaptedString key) const {
    if (isNull())
      return iterator();
    return findKey(key, data_, resources_);
  }

  template <typename TAdaptedString>
  static iterator findKey(TAdaptedString key, VariantData*, ResourceManager*);
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
