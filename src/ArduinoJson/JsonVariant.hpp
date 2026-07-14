// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Data/JsonVariantContent.hpp"
#include "Data/JsonVariantType.hpp"
#include "JsonVariantBase.hpp"
#include "RawJson.hpp"
#include "Serialization/JsonPrintable.hpp"

#include "TypeTraits/EnableIf.hpp"
#include "TypeTraits/IsChar.hpp"
#include "TypeTraits/IsFloatingPoint.hpp"
#include "TypeTraits/IsIntegral.hpp"
#include "TypeTraits/IsSame.hpp"
#include "TypeTraits/IsSignedIntegral.hpp"
#include "TypeTraits/IsUnsignedIntegral.hpp"
#include "TypeTraits/RemoveConstReference.hpp"
#include "TypeTraits/RemoveReference.hpp"

#include "TypeTraits/Or.hpp"

#include <cstddef>

namespace ArduinoJson {

// Forward declarations.
class JsonArray;
class JsonObject;

// A variant that can be a any value serializable to a JSON value.
//
// It can be set to:
// - a null
// - a boolean
// - a char, short, int, long or long long (signed or unsigned)
// - a const char* string, either typed as raw or plain string
// - a reference to a JsonArray or JsonObject
class JsonVariant : public Internals::JsonVariantBase<JsonVariant> {
  template <typename Print>
  friend class Internals::JsonSerializer;

  // The current type of the variant
  Internals::JsonVariantType _type;

  // The various alternatives for the value of the variant.
  Internals::JsonVariantContent _content;

 public:
  // Creates an empty variant by default
  JsonVariant() :
    _type(Internals::JsonVariantType::JSON_UNDEFINED),
    _content()
  {}

  // Create a JsonVariant containing a null value
  JsonVariant(JsonNull) :
    _type(Internals::JsonVariantType::JSON_NULL),
    _content(JsonNull{})
  {}

  // Create a JsonVariant containing a boolean value.
  // It will be serialized as "true" or "false" in JSON.
  JsonVariant(bool value) :
    _type(Internals::JsonVariantType::JSON_BOOLEAN),
    _content(static_cast<Internals::JsonUInt>(value))
  {}

  // Create a JsonVariant containing a floating point value.
  // JsonVariant(float value);
  // JsonVariant(double value);
  template <typename T>
  JsonVariant(T value,
      typename Internals::EnableIf<Internals::IsFloatingPoint<T>::value>::type* = nullptr) :

    _type(Internals::JsonVariantType::JSON_FLOAT),
    _content(static_cast<Internals::JsonFloat>(value))
  {}

  // Create a JsonVariant containing an integer value.
  // JsonVariant(char)
  // JsonVariant(signed char)
  // JsonVariant(signed short)
  // JsonVariant(signed int)
  // JsonVariant(signed long)
  // JsonVariant(signed long long)
  template <typename T>
  JsonVariant(T value,
      typename Internals::EnableIf<Internals::IsSignedIntegral<T>::value>::type* = nullptr) :

    _type((value >= 0)
            ? Internals::JsonVariantType::JSON_POSITIVE_INTEGER
            : Internals::JsonVariantType::JSON_NEGATIVE_INTEGER),
    _content((value >= 0)
            ? static_cast<Internals::JsonUInt>(value)
            : -static_cast<Internals::JsonUInt>(value))
  {}
  // JsonVariant(unsigned short)
  // JsonVariant(unsigned int)
  // JsonVariant(unsigned long)
  // JsonVariant(unsigned long long)
  template <typename T>
  JsonVariant(T value,
      typename Internals::EnableIf<
        Internals::And<
          Internals::IsUnsignedIntegral<T>,
          Internals::Not<Internals::IsSame<T, bool>>>::value>::type* = nullptr) :

    _type(Internals::JsonVariantType::JSON_POSITIVE_INTEGER),
    _content(static_cast<Internals::JsonUInt>(value))
  {}

  // Create a JsonVariant containing a string.
  // CAUTION: variant only stores the pointer, it does not manage its lifetime
  // JsonVariant(const char*);
  // JsonVariant(const signed char*);
  // JsonVariant(const unsigned char*);
  template <typename TChar>
  JsonVariant(
      const TChar *value,
      typename Internals::EnableIf<Internals::IsChar<TChar>::value>::type* = nullptr) :

    _type(Internals::JsonVariantType::JSON_STRING),
    _content(reinterpret_cast<const char *>(value))
 {}

  // Create a JsonVariant containing an unparsed string
  template <typename T>
  JsonVariant(Internals::RawJsonString<T> value) :
    _type(Internals::JsonVariantType::JSON_UNPARSED),
    _content(value.get())
  {}

  // Create a JsonVariant containing a reference to an array.
  // CAUTION: we are lying about constness, because the array can be modified if
  // the variant is converted back to a JsonArray&
  JsonVariant(const JsonArray &array);

  // Create a JsonVariant containing a reference to an object.
  // CAUTION: we are lying about constness, because the object can be modified
  // if the variant is converted back to a JsonObject&
  JsonVariant(const JsonObject &object);

  // Get the variant as the specified type.
  //
  // char as<char>() const;
  // signed char as<signed char>() const;
  // signed short as<signed short>() const;
  // signed int as<signed int>() const;
  // signed long as<signed long>() const;
  // signed long long as<signed long long>() const;
  // unsigned char as<unsigned char>() const;
  // unsigned short as<unsigned short>() const;
  // unsigned int as<unsigned int>() const;
  // unsigned long as<unsigned long>() const;
  // unsigned long long as<unsigned long long>() const;
  template <typename T>
  const typename Internals::EnableIf<
        Internals::And<typename Internals::IsIntegral<T>,
                       typename Internals::Not<Internals::IsSame<T, bool>>>::value,
        T>::type
  as() const {
    return variantAsInteger<T>();
  }
  // bool as<bool>() const
  template <typename T>
  const typename Internals::EnableIf<Internals::IsSame<T, bool>::value, T>::type
  as() const {
    return variantAsBoolean();
  }
  //
  // double as<double>() const;
  // float as<float>() const;
  template <typename T>
  const typename Internals::EnableIf<Internals::IsFloatingPoint<T>::value,
                                     T>::type
  as() const {
    return variantAsFloat<T>();
  }
  //
  // const char* as<const char*>() const;
  // const char* as<char*>() const;
  template <typename T>
  typename Internals::EnableIf<
    Internals::Or<Internals::IsSame<T, const char *>,
                  Internals::IsSame<T, char *>>::value,
    const char *>::type
  as() const {
    return variantAsString();
  }
  //
  // allow `const char*` conversion for types that could hold a reference to our data
  template <typename T>
  typename Internals::EnableIf<
    Internals::And<Internals::Not<Internals::IsSame<T, const char *>>,
                   Internals::Not<Internals::IsSame<T, char *>>,
                   Internals::CanReference<Internals::StringTraits<T>>>::value,
    T>::type
  as() const {
    using reference_for = typename Internals::StringTraits<T>::Reference;
    return reference_for::Operator(variantAsString());
  }

  // Any string type that is implemented in Internals::StringTraits and provides Append implementation
  template <typename T>
  typename Internals::EnableIf<Internals::HasAppend<Internals::StringTraits<T>>::value, T>::type
  as() const {
    T out;

    using append_to = typename Internals::StringTraits<T>::Append;
    if (variantIsString()) {
      auto* cstr = variantAsString();
      if (cstr)
        append_to::Operator(out, cstr);
    } else
      printTo(out);

    return out;
  }
  //
  // JsonArray& as<JsonArray> const;
  // JsonArray& as<JsonArray&> const;
  template <typename T>
  typename Internals::EnableIf<
      Internals::IsSame<typename Internals::RemoveReference<T>::type,
                        JsonArray>::value,
      JsonArray &>::type
  as() const {
    return variantAsArray();
  }
  //
  // const JsonArray& as<const JsonArray&> const;
  template <typename T>
  typename Internals::EnableIf<
      Internals::IsSame<typename Internals::RemoveReference<T>::type,
                        const JsonArray>::value,
      const JsonArray &>::type
  as() const {
    return variantAsArray();
  }
  //
  // JsonObject& as<JsonObject> const;
  // JsonObject& as<JsonObject&> const;
  template <typename T>
  typename Internals::EnableIf<
      Internals::IsSame<typename Internals::RemoveReference<T>::type,
                        JsonObject>::value,
      JsonObject &>::type
  as() const {
    return variantAsObject();
  }
  //
  // JsonObject& as<const JsonObject> const;
  // JsonObject& as<const JsonObject&> const;
  template <typename T>
  typename Internals::EnableIf<
      Internals::IsSame<typename Internals::RemoveReference<T>::type,
                        const JsonObject>::value,
      const JsonObject &>::type
  as() const {
    return variantAsObject();
  }
  //
  // JsonVariant as<JsonVariant> const;
  template <typename T>
  typename Internals::EnableIf<Internals::IsSame<T, JsonVariant>::value,
                               T>::type
  as() const {
    return *this;
  }

  // Tells weither the variant has the specified type.

  // Contains null type or is a raw "null" string
  //
  // bool is<JsonNull>() const;
  template <typename T>
  typename Internals::EnableIf<Internals::IsSame<T, JsonNull>::value, bool>::type
  is() const {
    return variantIsNull();
  }

  // Returns true if the variant has type type T, false otherwise.
  //
  // bool is<char>() const;
  // bool is<signed char>() const;
  // bool is<signed short>() const;
  // bool is<signed int>() const;
  // bool is<signed long>() const;
  // bool is<signed long long>() const;
  // bool is<unsigned char>() const;
  // bool is<unsigned short>() const;
  // bool is<unsigned int>() const;
  // bool is<unsigned long>() const;
  // bool is<unsigned long long>() const;
  template <typename T>
  typename Internals::EnableIf<
        Internals::And<typename Internals::IsIntegral<T>,
                       typename Internals::Not<Internals::IsSame<T, bool>>>::value,
  bool>::type
  is() const {
    return variantIsInteger();
  }
  //
  // bool is<double>() const;
  // bool is<float>() const;
  template <typename T>
  typename Internals::EnableIf<Internals::IsFloatingPoint<T>::value, bool>::type
  is() const {
    return variantIsFloat();
  }
  //
  // bool is<bool>() const
  template <typename T>
  typename Internals::EnableIf<Internals::IsSame<T, bool>::value, bool>::type
  is() const {
    return variantIsBoolean();
  }
  //
  // bool is<char*>() const;
  // bool is<const char*>() const;
  //
  // Also supports any other string type that is implemented in Internals::StringTraits and provides Append implementation
  template <typename T>
  typename Internals::EnableIf<
      Internals::Or<Internals::IsSame<T, const char *>,
                    Internals::IsSame<T, char *>,
                    Internals::HasAppend<Internals::StringTraits<T>>>::value,
      bool>::type
  is() const {
    return variantIsString();
  }
  //
  // bool is<JsonArray> const;
  // bool is<JsonArray&> const;
  // bool is<const JsonArray&> const;
  template <typename T>
  typename Internals::EnableIf<
      Internals::IsSame<typename Internals::RemoveConstReference<T>::type,
                        JsonArray>::value,
      bool>::type
  is() const {
    return variantIsArray();
  }
  //
  // bool is<JsonObject> const;
  // bool is<JsonObject&> const;
  // bool is<const JsonObject&> const;
  template <typename T>
  typename Internals::EnableIf<
      Internals::IsSame<typename Internals::RemoveConstReference<T>::type,
                        JsonObject>::value,
      bool>::type
  is() const {
    return variantIsObject();
  }

  //
  // bool is<std::nullptr_t> const;
  // Explicitly deleted to avoid bogus T* conversion checks
  template <typename T>
  typename Internals::EnableIf<
      Internals::IsSame<T, std::nullptr_t>::value,
      bool>::type
  is() const = delete;

  // Returns true if the variant has a value
  bool success() const;

 private:
  JsonArray &variantAsArray() const;
  JsonObject &variantAsObject() const;

  bool variantAsBoolean() const;
  const char *variantAsString() const;

  template <typename T>
  T variantAsFloat() const;

  template <typename T>
  T variantAsInteger() const;

  bool variantIsUndefined() const {
    return _type == Internals::JsonVariantType::JSON_UNDEFINED;
  }

  bool variantIsNull() const;
  bool variantIsBoolean() const;
  bool variantIsFloat() const;
  bool variantIsInteger() const;
  bool variantIsArray() const {
    return _type == Internals::JsonVariantType::JSON_ARRAY;
  }

  bool variantIsObject() const {
    return _type == Internals::JsonVariantType::JSON_OBJECT;
  }

  bool variantIsString() const {
    return _type == Internals::JsonVariantType::JSON_STRING ||
           _type == Internals::JsonVariantType::JSON_UNPARSED;
  }
};

}  // namespace ArduinoJson
