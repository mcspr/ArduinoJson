// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Serialization/JsonPrintable.hpp"

#include "JsonImplicitConversions.hpp"

#include "JsonVariantComparisons.hpp"
#include "JsonVariantOr.hpp"

#include "TypeTraits/EnableIf.hpp"
#include "TypeTraits/IsChar.hpp"
#include "TypeTraits/IsFloatingPoint.hpp"
#include "TypeTraits/IsIntegral.hpp"
#include "TypeTraits/IsSame.hpp"
#include "TypeTraits/IsSignedIntegral.hpp"
#include "TypeTraits/IsUnsignedIntegral.hpp"
#include "TypeTraits/RemoveReference.hpp"
#include "TypeTraits/Or.hpp"

#include "Data/JsonNull.hpp"
#include "Data/JsonVariantContent.hpp"
#include "Data/JsonVariantType.hpp"

#include "RawJson.hpp"
#include "JsonString.hpp"

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
class JsonVariant :
    public Internals::JsonPrintable<JsonVariant>,
    public Internals::JsonImplicitConversions<JsonVariant>,
    public Internals::JsonVariantComparisons<JsonVariant>,
    public Internals::JsonVariantOr<JsonVariant>,
    public Internals::JsonVariantTag {

 private:
  template <typename>
  friend class Internals::JsonSerializer;

  // The various alternatives for the value of the variant
  Internals::JsonVariantContent _content;

 public:
  // Creates an empty variant by default
  JsonVariant() noexcept :
    _content()
  {}

  // Create a JsonVariant containing a null value
  JsonVariant(JsonNull) noexcept :
    _content(JsonNull{})
  {}

  // Create a JsonVariant containing a boolean value.
  // It will be serialized as "true" or "false" in JSON.
  JsonVariant(bool value) noexcept :
    _content(value)
  {}

  // Create a JsonVariant containing a floating point value.
  // JsonVariant(float value);
  // JsonVariant(double value);
  template <typename T, typename Internals::EnableIf<
    Internals::IsFloatingPoint<T>::value>::type* = nullptr>
  JsonVariant(T value) noexcept :
    _content(static_cast<Internals::JsonFloat>(value))
  {}

  // Create a JsonVariant containing an integer value.
  // JsonVariant(char)
  // JsonVariant(signed char)
  // JsonVariant(signed short)
  // JsonVariant(signed int)
  // JsonVariant(signed long)
  // JsonVariant(signed long long)
  template <typename T, typename Internals::EnableIf<
    Internals::IsSignedIntegral<T>::value>::type* = nullptr>
  JsonVariant(T value) noexcept :
    _content(static_cast<Internals::JsonInteger>(value))
  {}

  // JsonVariant(unsigned short)
  // JsonVariant(unsigned int)
  // JsonVariant(unsigned long)
  // JsonVariant(unsigned long long)
  template <typename T, typename Internals::EnableIf<
    Internals::And<
      Internals::IsUnsignedIntegral<T>,
      Internals::Not<Internals::IsSame<T, bool>>>::value>::type* = nullptr>
  JsonVariant(T value) noexcept :
    _content(static_cast<Internals::JsonUnsignedInteger>(value))
  {}

  // Create a JsonVariant containing a string.
  // CAUTION: variant only stores the pointer, it does not manage its lifetime
  // JsonVariant(const char*);
  // JsonVariant(const signed char*);
  // JsonVariant(const unsigned char*);
  template <typename TChar, typename Internals::EnableIf<
    Internals::IsChar<TChar>::value>::type* = nullptr>
  JsonVariant(const TChar *value) noexcept :
    _content(reinterpret_cast<const char *>(value))
 {}

  // Internally used overload when data is managed by JsonVariant
  // JsonVariant(StringBufferValue{const char[]});
  JsonVariant(Internals::JsonVariantContent::StringBufferValue buffer_value) :
    _content(buffer_value)
 {}

  // Create a JsonVariant containing an unparsed string
  // JsonVariant(RawJson(...))
  template <typename T>
  JsonVariant(Internals::RawJsonString<T> value) noexcept :
    _content(value.get(), false)
  {}

  // Convert JsonString variant-like to a JsonVariant containing string data
  // CAUTION: usually only called internally, prefer other constructors
  JsonVariant(Internals::JsonString, bool parsed) noexcept;

  // Create a JsonVariant containing a reference to an array.
  JsonVariant(const JsonArray &array) noexcept;
  JsonVariant(JsonArray &array) noexcept;

  // Create a JsonVariant containing a reference to an object.
  JsonVariant(const JsonObject &object) noexcept;
  JsonVariant(JsonObject &object) noexcept;

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
  typename Internals::EnableIf<
    Internals::And<typename Internals::IsIntegral<T>,
                   typename Internals::Not<Internals::IsSame<T, bool>>>::value,
    T>::type
  as() const {
    return variantAsInteger<T>();
  }
  // bool as<bool>() const
  template <typename T>
  typename Internals::EnableIf<
    Internals::IsSame<T, bool>::value, T>::type
  as() const {
    return variantAsBoolean();
  }
  //
  // double as<double>() const;
  // float as<float>() const;
  template <typename T>
  typename Internals::EnableIf<
    Internals::IsFloatingPoint<T>::value, T>::type
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
    const char *>::type
  as() const {
    using reference_for = typename Internals::StringTraits<T>::Reference;
    return reference_for::Operator(variantAsString());
  }

  // Any string type that is implemented in Internals::StringTraits and provides Append implementation
  template <typename T>
  typename Internals::EnableIf<
    Internals::HasAppend<Internals::StringTraits<T>>::value, T>::type
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
  // const JsonArray& as<const JsonArray> const;
  // const JsonArray& as<const JsonArray&> const;
  template <typename T>
  typename Internals::EnableIf<
    Internals::IsSame<typename Internals::RemoveReference<T>::type,
                      const JsonArray>::value,
    const JsonArray &>::type
  as() const {
    return variantAsConstArray();
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
    return variantAsMutableArray();
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
    return variantAsConstObject();
  }
  //
  // JsonObject& as<JsonObject>;
  // JsonObject& as<JsonObject&>;
  template <typename T>
  typename Internals::EnableIf<
    Internals::IsSame<typename Internals::RemoveReference<T>::type,
                      JsonObject>::value,
    JsonObject &>::type
  as() const {
    return variantAsMutableObject();
  }

  //
  // JsonVariant as<JsonVariant> const;
  template <typename T>
  typename Internals::EnableIf<
    Internals::IsSame<T, JsonVariant>::value, T>::type
  as() const {
    return *this;
  }

  // Tells weither the variant has the specified type.

  // Contains null type or is a raw "null" string
  //
  // bool is<JsonNull>() const;
  template <typename T>
  typename Internals::EnableIf<
    Internals::IsSame<T, JsonNull>::value, bool>::type
  is() const {
    return variantMaybeNull();
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
    return variantMaybeInteger();
  }
  //
  // bool is<double>() const;
  // bool is<float>() const;
  template <typename T>
  typename Internals::EnableIf<
    Internals::IsFloatingPoint<T>::value, bool>::type
  is() const {
    return variantMaybeFloat();
  }
  //
  // bool is<bool>() const
  template <typename T>
  typename Internals::EnableIf<
    Internals::IsSame<T, bool>::value, bool>::type
  is() const {
    return variantMaybeBoolean();
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
  // bool is<const JsonArray> const;
  // bool is<const JsonArray&> const;
  template <typename T>
  typename Internals::EnableIf<
    Internals::IsSame<typename Internals::RemoveReference<T>::type,
                      const JsonArray>::value,
    bool>::type
  is() const {
    return variantIsArray();
  }

  //
  // bool is<JsonArray> const;
  // bool is<JsonArray&> const;
  template <typename T>
  typename Internals::EnableIf<
    Internals::IsSame<typename Internals::RemoveReference<T>::type,
                      JsonArray>::value,
    bool>::type
  is() const {
    return variantIsMutableArray();
  }

  //
  // bool is<const JsonObject> const;
  // bool is<const JsonObject&> const;
  template <typename T>
  typename Internals::EnableIf<
    Internals::IsSame<typename Internals::RemoveReference<T>::type,
                      const JsonObject>::value,
    bool>::type
  is() const {
    return variantIsObject();
  }

  //
  // bool is<JsonObject> const;
  // bool is<JsonObject&> const;
  template <typename T>
  typename Internals::EnableIf<
    Internals::IsSame<typename Internals::RemoveReference<T>::type,
                      JsonObject>::value,
    bool>::type
  is() const {
    return variantIsMutableObject();
  }

  //
  // bool is<std::nullptr_t> const;
  // Explicitly deleted to avoid bogus T* conversion checks
  template <typename T>
  typename Internals::EnableIf<
    Internals::IsSame<T, std::nullptr_t>::value, bool>::type
  is() const = delete;

  // Returns true if the variant has a value
  bool success() const;

 private:
  // internal ctor for existing contents
  explicit JsonVariant(Internals::JsonVariantContent content) noexcept :
    _content(content)
  {}

  // sometimes types are implicitly convertible, even when they don't exactly match

  bool variantIsUndefined() const {
    return _content.undefined.type ==
      Internals::JsonVariantType::JSON_UNDEFINED;
  }

  bool variantMaybeNull() const; // for types other than JSON_NULL
  bool variantIsNull() const {
    return _content.null.type ==
      Internals::JsonVariantType::JSON_NULL;
  }

  bool variantMaybeBoolean() const; // for types other than JSON_BOOLEAN
  bool variantIsBoolean() const {
    return _content.asBoolean.type ==
      Internals::JsonVariantType::JSON_BOOLEAN;
  }

  bool variantMaybeFloat() const; // for types other that JSON_FLOAT
  bool variantIsFloat() const {
    return _content.asFloat.type ==
      Internals::JsonVariantType::JSON_FLOAT;
  }

  bool variantIsSignedInteger() const {
    return _content.asSignedInteger.type ==
      Internals::JsonVariantType::JSON_SIGNED_INTEGER;
  }

  bool variantIsUnsignedInteger() const {
    return _content.asUnsignedInteger.type ==
      Internals::JsonVariantType::JSON_UNSIGNED_INTEGER;
  }

  bool variantMaybeInteger() const; // for types other than JSON_{SIGNED,UNSIGNED}_INTEGER
  bool variantIsInteger() const {
    return variantIsSignedInteger() || variantIsUnsignedInteger();
  }

  // not implicitly convertible, match the exact type

  bool variantIsConstObject() const {
    return _content.asObject.type ==
      Internals::JsonVariantType::JSON_CONST_OBJECT;
  }

  bool variantIsMutableObject() const {
    return _content.asObject.type ==
      Internals::JsonVariantType::JSON_MUTABLE_OBJECT;
  }

  bool variantIsObject() const {
    return variantIsConstObject() || variantIsMutableObject();
  }

  bool variantIsConstArray() const {
    return _content.asArray.type ==
      Internals::JsonVariantType::JSON_CONST_ARRAY;
  }

  bool variantIsMutableArray() const {
    return _content.asArray.type ==
      Internals::JsonVariantType::JSON_MUTABLE_ARRAY;
  }

  bool variantIsArray() const {
    return variantIsConstArray() || variantIsMutableArray();
  }

  bool variantIsString() const {
    return _content.asStringPointer.type ==
      Internals::JsonVariantType::JSON_STRING ||
           _content.asStringPointer.type ==
      Internals::JsonVariantType::JSON_STRING_BUFFER;
  }

  // return exact or implicitly converted values

  bool variantAsBoolean() const;

  const JsonObject& variantAsConstObject() const;
  JsonObject& variantAsMutableObject() const;

  const JsonArray& variantAsConstArray() const;
  JsonArray& variantAsMutableArray() const;

  template <typename T>
  T variantAsFloat() const;

  template <typename T>
  T variantAsInteger() const;

  const char *variantAsString() const;
};

}  // namespace ArduinoJson
