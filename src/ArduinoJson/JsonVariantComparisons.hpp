// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "StringTraits/StringTraits.hpp"

#include "Data/JsonFloat.hpp"
#include "Data/JsonInteger.hpp"
#include "Data/JsonVariantAs.hpp"

#include "TypeTraits/EnableIf.hpp"
#include "TypeTraits/IsVariant.hpp"

#include "TypeTraits/And.hpp"
#include "TypeTraits/Not.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename TImpl>
class JsonVariantComparisons;

struct JsonVariantComparisonsHelper {
  template <typename TFirst, typename TSecond>
  static bool equals(const JsonVariantComparisons<TFirst>&,
                     const JsonVariantComparisons<TSecond>&);
};

template <typename TImpl>
class JsonVariantComparisons {
 public:
  template <typename TOther>
  friend bool operator==(const JsonVariantComparisons &variant,
                         const JsonVariantComparisons<TOther> &comparand) {
    return variant.equals(comparand);
  }

  template <typename TComparand>
  friend typename EnableIf<!IsVariant<TComparand>::value, bool>::type
  operator==(const JsonVariantComparisons &variant, const TComparand &comparand) {
    return variant.equals(comparand);
  }

  template <typename TComparand>
  friend typename EnableIf<!IsVariant<TComparand>::value, bool>::type
  operator==(const TComparand &comparand, const JsonVariantComparisons &variant) {
    return variant.equals(comparand);
  }

  template <typename TOther>
  friend bool operator!=(const JsonVariantComparisons &variant,
                         const JsonVariantComparisons<TOther> &comparand) {
    return !variant.equals(comparand);
  }

  template <typename TComparand>
  friend typename EnableIf<!IsVariant<TComparand>::value, bool>::type
  operator!=(const JsonVariantComparisons &variant, const TComparand &comparand) {
    return !variant.equals(comparand);
  }

  template <typename TComparand>
  friend typename EnableIf<!IsVariant<TComparand>::value, bool>::type
  operator!=(const TComparand &comparand, const JsonVariantComparisons &variant) {
    return !variant.equals(comparand);
  }

  template <typename TComparand>
  friend bool operator<=(const JsonVariantComparisons &left, const TComparand &right) {
    return left.as<TComparand>() <= right;
  }

  template <typename TComparand>
  friend bool operator<=(const TComparand &comparand,
                         const JsonVariantComparisons &variant) {
    return comparand <= variant.as<TComparand>();
  }

  template <typename TComparand>
  friend bool operator>=(const JsonVariantComparisons &variant,
                         const TComparand &comparand) {
    return variant.as<TComparand>() >= comparand;
  }

  template <typename TComparand>
  friend bool operator>=(const TComparand &comparand,
                         const JsonVariantComparisons &variant) {
    return comparand >= variant.as<TComparand>();
  }

  template <typename TComparand>
  friend bool operator<(const JsonVariantComparisons &varian,
                        const TComparand &comparand) {
    return varian.as<TComparand>() < comparand;
  }

  template <typename TComparand>
  friend bool operator<(const TComparand &comparand,
                        const JsonVariantComparisons &variant) {
    return comparand < variant.as<TComparand>();
  }

  template <typename TComparand>
  friend bool operator>(const JsonVariantComparisons &variant,
                        const TComparand &comparand) {
    return variant.as<TComparand>() > comparand;
  }

  template <typename TComparand>
  friend bool operator>(const TComparand &comparand,
                        const JsonVariantComparisons &variant) {
    return comparand > variant.as<TComparand>();
  }

 private:
  const TImpl *impl() const {
    return static_cast<const TImpl *>(this);
  }

  template <typename T>
  typename JsonVariantAs<T>::type as() const {
    return impl()->template as<typename JsonVariantAs<T>::type>();
  }

  template <typename T>
  bool is() const {
    return impl()->template is<typename JsonVariantAs<T>::type>();
  }

  template <typename TString>
  typename EnableIf<HasEquals<StringTraits<TString>>::value, bool>::type
  equals(const TString &comparand) const {
    return StringTraits<TString>::Equals::Operator(comparand, as<const char *>());
  }

  template <typename TComparand>
  typename EnableIf<
    And<Not<IsVariant<TComparand>>,
        Not<HasEquals<StringTraits<TComparand>>>>::value,
    bool>::type
  equals(const TComparand &comparand) const {
    return as<TComparand>() == comparand;
  }

  template <typename TVariant2>
  bool equals(const JsonVariantComparisons<TVariant2> &right) const {
    return JsonVariantComparisonsHelper::equals(*this, right);
  }

  friend struct JsonVariantComparisonsHelper;
};

template <typename TFirst, typename TSecond>
inline bool JsonVariantComparisonsHelper::equals(
    const JsonVariantComparisons<TFirst>& left,
    const JsonVariantComparisons<TSecond>& right)
{
    if (left.template is<bool>() && right.template is<bool>())
      return left.template as<bool>() == right.template as<bool>();
    if (left.template is<JsonInteger>() && right.template is<JsonInteger>())
      return left.template as<JsonInteger>() == right.template as<JsonInteger>();
    if (left.template is<JsonFloat>() && right.template is<JsonFloat>())
      return left.template as<JsonFloat>() == right.template as<JsonFloat>();
    if (left.template is<JsonArray>() && right.template is<JsonArray>())
      return left.template as<JsonArray>() == right.template as<JsonArray>();
    if (left.template is<JsonObject>() && right.template is<JsonObject>())
      return left.template as<JsonObject>() == right.template as<JsonObject>();
    if (left.template is<const char *>() && right.template is<const char *>())
      return Strings::Equals::Operator(
        left.template as<const char *>(), right.template as<const char *>());

    return false;
}

}  // namespace Internals
}  // namespace ArduinoJson
