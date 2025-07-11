// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Object/JsonPair.hpp>

ARDUINOJSON_BEGIN_PUBLIC_NAMESPACE

class JsonObjectIterator {
  friend class JsonObject;

 public:
  JsonObjectIterator() {}

  explicit JsonObjectIterator(const detail::VariantImpl::iterator& iterator)
      : iterator_(iterator) {}

  JsonPair operator*() const {
    return JsonPair(iterator_);
  }
  Ptr<JsonPair> operator->() {
    return operator*();
  }

  bool operator==(const JsonObjectIterator& other) const {
    return iterator_ == other.iterator_;
  }

  bool operator!=(const JsonObjectIterator& other) const {
    return iterator_ != other.iterator_;
  }

  JsonObjectIterator& operator++() {
    iterator_.next();  // key
    iterator_.next();  // value
    return *this;
  }

 private:
  detail::VariantImpl::iterator iterator_;
};

class JsonObjectConstIterator {
  friend class JsonObject;

 public:
  JsonObjectConstIterator() {}

  explicit JsonObjectConstIterator(
      const detail::VariantImpl::iterator& iterator)
      : iterator_(iterator) {}

  JsonPairConst operator*() const {
    return JsonPairConst(iterator_);
  }
  Ptr<JsonPairConst> operator->() {
    return operator*();
  }

  bool operator==(const JsonObjectConstIterator& other) const {
    return iterator_ == other.iterator_;
  }

  bool operator!=(const JsonObjectConstIterator& other) const {
    return iterator_ != other.iterator_;
  }

  JsonObjectConstIterator& operator++() {
    iterator_.next();  // key
    iterator_.next();  // value
    return *this;
  }

 private:
  detail::VariantImpl::iterator iterator_;
};

ARDUINOJSON_END_PUBLIC_NAMESPACE
