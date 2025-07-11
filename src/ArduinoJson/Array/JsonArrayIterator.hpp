// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Collection/CollectionIterator.hpp>
#include <ArduinoJson/Variant/JsonVariant.hpp>

ARDUINOJSON_BEGIN_PUBLIC_NAMESPACE

template <typename T>
class Ptr {
 public:
  Ptr(T value) : value_(value) {}

  T* operator->() {
    return &value_;
  }

  T& operator*() {
    return value_;
  }

 private:
  T value_;
};

class JsonArrayIterator {
  friend class JsonArray;

 public:
  JsonArrayIterator() {}
  explicit JsonArrayIterator(const detail::VariantImpl::iterator& iterator)
      : iterator_(iterator) {}

  JsonVariant operator*() {
    return JsonVariant(*iterator_);
  }
  Ptr<JsonVariant> operator->() {
    return operator*();
  }

  bool operator==(const JsonArrayIterator& other) const {
    return iterator_ == other.iterator_;
  }

  bool operator!=(const JsonArrayIterator& other) const {
    return iterator_ != other.iterator_;
  }

  JsonArrayIterator& operator++() {
    iterator_.next();
    return *this;
  }

 private:
  detail::VariantImpl::iterator iterator_;
};

class JsonArrayConstIterator {
  friend class JsonArray;

 public:
  JsonArrayConstIterator() {}
  explicit JsonArrayConstIterator(const detail::VariantImpl::iterator& iterator)
      : iterator_(iterator) {}

  JsonVariantConst operator*() const {
    return JsonVariantConst(*iterator_);
  }
  Ptr<JsonVariantConst> operator->() {
    return operator*();
  }

  bool operator==(const JsonArrayConstIterator& other) const {
    return iterator_ == other.iterator_;
  }

  bool operator!=(const JsonArrayConstIterator& other) const {
    return iterator_ != other.iterator_;
  }

  JsonArrayConstIterator& operator++() {
    iterator_.next();
    return *this;
  }

 private:
  mutable detail::VariantImpl::iterator iterator_;
};

ARDUINOJSON_END_PUBLIC_NAMESPACE
