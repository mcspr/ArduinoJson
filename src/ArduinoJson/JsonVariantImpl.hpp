// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "ArduinoJson/Data/JsonVariantDefault.hpp"
#include "ArduinoJson/Data/ValueSaver.hpp"
#include "ArduinoJson/Numbers/convertNumber.hpp"
#include "Configuration.hpp"

#include "Data/JsonLiterals.hpp"
#include "Data/JsonFloat.hpp"
#include "Data/JsonInteger.hpp"
#include "Data/JsonNull.hpp"
#include "Data/JsonVariantContent.hpp"
#include "Data/JsonStringPointer.hpp"
#include "Data/JsonVariantType.hpp"

#include "JsonArray.hpp"
#include "JsonObject.hpp"
#include "JsonVariant.hpp"

#include "Strings/Strings.hpp"

#include "Numbers/isFloat.hpp"
#include "Numbers/parseFloat.hpp"

#include "Numbers/isInteger.hpp"
#include "Numbers/parseInteger.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename Source>
bool valueSaverDuplicate(JsonBuffer* buffer, JsonVariant& dst, Source src) {
  auto src_ref = MakeStringRef(src.get());
  if (ValueSaverIsNull<decltype(src_ref)>::Operator(src_ref))
    return false;

  typedef StringTraits<decltype(src_ref)> source_ref_traits;
  const auto length = source_ref_traits::Length::Operator(src_ref);

  typedef ValueStringDuplicate<Source> duplicate_traits;
  static constexpr auto is_raw_json = duplicate_traits::is_raw_json::value;

  typedef StringTraits<Source> source_traits;
  typedef JsonVariantContent::StringBufferValue buffer_type;
  static constexpr auto buffer_size = sizeof(buffer_type);

  if ((length + 1) <= buffer_size) {
    buffer_type tmp{};
    if (length)
      source_traits::Duplicate::Operator(&tmp.value[0], std::move(src), length);
    if (is_raw_json)
      dst = RawJson(tmp);
    else
      dst = tmp;
    return true;
  }

  auto* dup = source_traits::Duplicate::Operator(buffer, std::move(src));
  if (dup) {
    if (is_raw_json)
      dst = RawJson(dup);
    else
      dst = dup;
    return true;
  }

  return false;
}

inline bool JsonLiterals::isFalse(const char* str) {
  return Strings::Equals::Operator(str, False);
}

inline bool JsonLiterals::isTrue(const char* str) {
  return Strings::Equals::Operator(str, True);
}

inline bool JsonLiterals::isNull(const char* str) {
  return Strings::Equals::Operator(str, Null);
}

struct JsonVariantAsBoolean {
  template <typename TArgs>
  static bool Operator(TArgs&&...) {
    return defaultValue();
  }

  static bool Operator(bool value) {
    return value;
  }

  static bool Operator(JsonFloat value) {
    return value != JsonFloat(0);
  }

  static bool Operator(JsonInteger value) {
    return value != JsonInteger(0);
  }

  static bool Operator(JsonUnsignedInteger value) {
    return value != JsonUnsignedInteger(0);
  }

  static bool Operator(JsonStringPointer str) {
    if (!str.data || JsonLiterals::isFalse(str.data))
      return false;

    return true;
  }

 private:
  static bool defaultValue() {
    return JsonVariantDefault<bool>::get();
  }
};

struct JsonVariantAsObject {
  template <typename... TArgs>
  static JsonObject& Operator(TArgs&&...) {
    return defaultValue();
  }

  static JsonObject& Operator(JsonObject* object) {
    return *object;
  }

 private:
  static JsonObject& defaultValue() {
    return JsonVariantDefault<JsonObject>::get();
  }
};

struct JsonVariantAsArray {
  template <typename... TArgs>
  static JsonArray& Operator(TArgs&&...) {
    return defaultValue();
  }

  static JsonArray& Operator(JsonArray* array) {
    return *array;
  }

 private:
  static JsonArray& defaultValue() {
    return JsonVariantDefault<JsonArray>::get();
  }
};

template <typename TOut>
struct JsonVariantAsFloat {
  template <typename TIn>
  static TOut Operator(TIn value) {
    const auto result = convertNumber<TOut>(value);
    if (result)
      return result.value;

    return defaultValue();
  }

  static TOut Operator(JsonNull) {
    return defaultValue();
  }

  static TOut Operator(JsonObject*) {
    return defaultValue();
  }

  static TOut Operator(JsonArray*) {
    return defaultValue();
  }

  static TOut Operator(JsonStringPointer str) {
    const auto converted = Internals::parseFloat<TOut>(str.data);
    if (converted)
      return converted.value;

    return defaultValue();
  }

 private:
  static TOut defaultValue() {
    return JsonVariantDefault<TOut>::get();
  }
};

template <typename TOut>
struct JsonVariantAsInteger {
 public:
  template <typename TIn>
  static TOut Operator(TIn value) {
    const auto result = convertNumber<TOut>(value);
    if (result)
      return result.value;

    return defaultValue();
  }

  static TOut Operator(JsonNull) {
    return defaultValue();
  }

  static TOut Operator(JsonObject*) {
    return defaultValue();
  }

  static TOut Operator(JsonArray*) {
    return defaultValue();
  }

  static TOut Operator(JsonStringPointer str) {
    const auto converted = Internals::parseInteger<TOut>(str.data);
    if (converted)
      return converted.value;

    return defaultValue();
  }

 private:
  static TOut defaultValue() {
    return JsonVariantDefault<TOut>::get();
  }
};

struct JsonVariantMaybeNull {
  template <typename... TArgs>
  static bool Operator(TArgs&&...) {
    return defaultValue();
  }

  static bool Operator(JsonNull) {
    return true;
  }

  static bool Operator(JsonStringPointer str) {
    if (str.data && !str.parsed)
      return JsonLiterals::isNull(str.data);

    return defaultValue();
  }

 private:
  static bool defaultValue() {
    return false;
  }
};

struct JsonVariantMaybeBoolean {
  template <typename... TArgs>
  static bool Operator(TArgs&&...) {
    return defaultValue();
  }

  static bool Operator(bool) {
    return true;
  }

  static bool Operator(JsonStringPointer str) {
    if (str.data && !str.parsed)
      return JsonLiterals::isFalse(str.data) ||
             JsonLiterals::isTrue(str.data);

    return defaultValue();
  }

 private:
  static bool defaultValue() {
    return false;
  }
};

struct JsonVariantIsObject {
  template <typename... TArgs>
  static bool Operator(TArgs&&...) {
    return defaultValue();
  }

  static bool Operator(JsonObject*) {
    return true;
  }

 private:
  static bool defaultValue() {
    return false;
  }
};

struct JsonVariantIsArray {
  template <typename... TArgs>
  static bool Operator(TArgs&&...) {
    return defaultValue();
  }

  static bool Operator(JsonArray*) {
    return true;
  }

 private:
  static bool defaultValue() {
    return false;
  }
};

struct JsonVariantMaybeInteger {
  template <typename... TArgs>
  static bool Operator(TArgs&&...) {
    return defaultValue();
  }

  static bool Operator(JsonInteger) {
    return true;
  }

  static bool Operator(JsonUnsignedInteger) {
    return true;
  }

  static bool Operator(JsonStringPointer str) {
    return str.data && !str.parsed && isInteger(str.data);
  }

 private:
  static bool defaultValue() {
    return false;
  }
};

struct JsonVariantMaybeFloat {
  template <typename T>
  static bool Operator(T&&) {
    return false;
  }

  static bool Operator(JsonFloat) {
    return true;
  }

  static bool Operator(JsonInteger) {
    return true;
  }

  static bool Operator(JsonUnsignedInteger) {
    return true;
  }

  static bool Operator(JsonStringPointer str) {
    return str.data && !str.parsed && isFloat(str.data);
  }
};

struct JsonVariantSuccess {
  template <typename... TArgs>
  static bool Operator(TArgs&&...) {
    return true;
  }

  static bool Operator(JsonObject* object) {
    return object->success();
  }

  static bool Operator(JsonArray* array) {
    return array->success();
  }
};

struct JsonVariantMaybeString {
  template <typename T>
  static const char* Operator(T&&) {
    return nullptr;
  }

  static const char* Operator(JsonStringPointer str) {
    return str.data;
  }
};

// adapt different union base, which contains only a subset of possible values
struct JsonStringVisitor {
  explicit JsonStringVisitor(bool parsed) noexcept :
    _parsed(parsed)
  {}

  JsonVariantContent Operator(const char* pointer) noexcept {
    return JsonVariantContent(pointer, _parsed);
  }

  JsonVariantContent Operator(JsonVariantContent::StringBufferValue buffer) noexcept {
    return JsonVariantContent(buffer, _parsed);
  }

 private:
  bool _parsed;
};

}

inline JsonVariant::JsonVariant(Internals::JsonString other, bool parsed) noexcept :
  _content(other.visit<Internals::JsonVariantContent>(
    Internals::JsonStringVisitor(parsed)))
{}

inline JsonVariant::JsonVariant(const JsonArray &array) noexcept :
  _content(const_cast<JsonArray *>(std::addressof(array)))
{}

inline JsonVariant::JsonVariant(const JsonObject &object) noexcept :
  _content(const_cast<JsonObject *>(std::addressof(object)))
{}

template <typename R, typename T>
R JsonVariant::visit(T&& visitor) const {
  using Internals::JsonVariantType;

  if (_content.null.type == JsonVariantType::JSON_NULL) {
    return visitor.Operator(JsonNull{});

  } else if (_content.asBoolean.type == JsonVariantType::JSON_BOOLEAN) {
    return visitor.Operator(_content.asBoolean.value);

  } else if (_content.asObject.type == JsonVariantType::JSON_OBJECT) {
    return visitor.Operator(_content.asObject.pointer);

  } else if (_content.asArray.type == JsonVariantType::JSON_ARRAY) {
    return visitor.Operator(_content.asArray.pointer);

  } else if (_content.asFloat.type == JsonVariantType::JSON_FLOAT) {
    return visitor.Operator(_content.asFloat.value);

  } else if (_content.asSignedInteger.type == JsonVariantType::JSON_SIGNED_INTEGER) {
    return visitor.Operator(_content.asSignedInteger.value);

  } else if (_content.asUnsignedInteger.type == JsonVariantType::JSON_UNSIGNED_INTEGER) {
    return visitor.Operator(_content.asUnsignedInteger.value);

  } else if (_content.asStringPointer.type == JsonVariantType::JSON_STRING) {
    return visitor.Operator(Internals::JsonStringPointer{
      _content.asStringPointer.pointer, _content.asStringPointer.parsed});

  } else if (_content.asStringBuffer.type == JsonVariantType::JSON_STRING_BUFFER) {
    return visitor.Operator(Internals::JsonStringPointer{
      &_content.asStringBuffer.buffer.value[0], _content.asStringPointer.parsed});

  }

  return Internals::JsonVariantDefault<R>::get();
}

inline bool JsonVariant::variantAsBoolean() const {
  return visit<bool>(Internals::JsonVariantAsBoolean());
}

inline JsonObject &JsonVariant::variantAsObject() const {
  return visit<decltype(JsonObject::invalid())>(
    Internals::JsonVariantAsObject());
}

inline JsonArray &JsonVariant::variantAsArray() const {
  return visit<decltype(JsonArray::invalid())>(
    Internals::JsonVariantAsArray());
}

template <typename T>
inline T JsonVariant::variantAsFloat() const {
  return visit<T>(Internals::JsonVariantAsFloat<T>());
}

template <typename T>
inline T JsonVariant::variantAsInteger() const {
  return visit<T>(Internals::JsonVariantAsInteger<T>());
}

inline const char *JsonVariant::variantAsString() const {
  return visit<const char*>(Internals::JsonVariantMaybeString());
}

inline bool JsonVariant::variantMaybeNull() const {
  return visit<bool>(Internals::JsonVariantMaybeNull());
}

inline bool JsonVariant::variantMaybeBoolean() const {
  return visit<bool>(Internals::JsonVariantMaybeBoolean());
}

inline bool JsonVariant::variantMaybeInteger() const {
  return visit<bool>(Internals::JsonVariantMaybeInteger());
}

inline bool JsonVariant::variantMaybeFloat() const {
  return visit<bool>(Internals::JsonVariantMaybeFloat());
}

inline bool JsonVariant::success() const {
  return visit<bool>(Internals::JsonVariantSuccess());
}

#if ARDUINOJSON_ENABLE_STD_STREAM
inline std::ostream &operator<<(std::ostream &os, const JsonVariant &source) {
  return source.printTo(os);
}
#endif

}  // namespace ArduinoJson
