// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "ArduinoJson/Data/JsonVariantContent.hpp"
#include "ArduinoJson/Data/JsonVariantType.hpp"
#include "ArduinoJson/Data/JsonFloat.hpp"

#include "Configuration.hpp"

#include "JsonArray.hpp"
#include "JsonObject.hpp"
#include "JsonVariant.hpp"

#include "Numbers/isFloat.hpp"
#include "Numbers/parseFloat.hpp"

#include "Numbers/isInteger.hpp"
#include "Numbers/parseInteger.hpp"

#include <cstring> // for strcmp

namespace ArduinoJson {

inline JsonVariant::JsonVariant(const JsonArray &array) :
  _type(Internals::JsonVariantType::JSON_ARRAY),
  _content(const_cast<JsonArray *>(std::addressof(array)))
{}

inline JsonVariant::JsonVariant(const JsonObject &object) :
  _type(Internals::JsonVariantType::JSON_OBJECT),
  _content(const_cast<JsonObject *>(std::addressof(object)))
{}

inline JsonArray &JsonVariant::variantAsArray() const {
  if (_type == Internals::JsonVariantType::JSON_ARRAY)
    return *_content.asArray;

  return JsonArray::invalid();
}

inline JsonObject &JsonVariant::variantAsObject() const {
  if (_type == Internals::JsonVariantType::JSON_OBJECT)
    return *_content.asObject;

  return JsonObject::invalid();
}

template <typename T>
inline T JsonVariant::variantAsInteger() const {
  using Internals::JsonVariantType;

  switch (_type) {
    case JsonVariantType::JSON_UNDEFINED:
    case JsonVariantType::JSON_NULL:
    case JsonVariantType::JSON_OBJECT:
    case JsonVariantType::JSON_ARRAY:
      break;

    case JsonVariantType::JSON_BOOLEAN:
    case JsonVariantType::JSON_POSITIVE_INTEGER:
      return T(_content.asInteger);

    case JsonVariantType::JSON_NEGATIVE_INTEGER:
      return T(~_content.asInteger + 1);

    case JsonVariantType::JSON_FLOAT:
      return T(_content.asFloat);

    case JsonVariantType::JSON_STRING:
    case JsonVariantType::JSON_UNPARSED:
      if (_content.asString) {
        const auto converted = Internals::parseInteger<T>(_content.asString);
        if (converted)
          return converted.value;
      }
      break;
  }

  return T();
}

inline bool JsonVariant::variantAsBoolean() const {
  using Internals::JsonVariantType;

  switch (_type) {
    case JsonVariantType::JSON_UNDEFINED:
    case JsonVariantType::JSON_NULL:
      break;

    case JsonVariantType::JSON_OBJECT:
    case JsonVariantType::JSON_ARRAY:
      return success();

    case JsonVariantType::JSON_BOOLEAN:
    case JsonVariantType::JSON_POSITIVE_INTEGER:
    case JsonVariantType::JSON_NEGATIVE_INTEGER:
      return _content.asInteger != 0;

    case JsonVariantType::JSON_FLOAT:
      return _content.asFloat != Internals::JsonFloat(0);

    case JsonVariantType::JSON_STRING:
    case JsonVariantType::JSON_UNPARSED:
      if (!_content.asString || (strcmp(_content.asString, "false") == 0))
        return false;

      return true;
  }

  return false;
}

inline const char *JsonVariant::variantAsString() const {
  using Internals::JsonVariantType;

  if (_type == JsonVariantType::JSON_STRING ||
      _type == JsonVariantType::JSON_UNPARSED)
    return _content.asString;

  return nullptr;
}

template <typename T>
inline T JsonVariant::variantAsFloat() const {
  using Internals::JsonVariantType;

  switch (_type) {
    case JsonVariantType::JSON_UNDEFINED:
    case JsonVariantType::JSON_NULL:
    case JsonVariantType::JSON_OBJECT:
    case JsonVariantType::JSON_ARRAY:
      break;

    case JsonVariantType::JSON_BOOLEAN:
    case JsonVariantType::JSON_POSITIVE_INTEGER:
      return static_cast<T>(_content.asInteger);

    case JsonVariantType::JSON_NEGATIVE_INTEGER:
      return -static_cast<T>(_content.asInteger);

    case JsonVariantType::JSON_FLOAT:
      return T(_content.asFloat);

    case JsonVariantType::JSON_STRING:
    case JsonVariantType::JSON_UNPARSED:
      if (_content.asString) {
        const auto converted = Internals::parseFloat<T>(_content.asString);
        if (converted)
          return converted.value;
      }
      break;
  }

  return T(0);
}

inline bool JsonVariant::variantIsNull() const {
  return (_type == Internals::JsonVariantType::JSON_NULL ||
        ((_type == Internals::JsonVariantType::JSON_UNPARSED) &&
         _content.asString &&
         (0 == strcmp("null", _content.asString))));
}

inline bool JsonVariant::variantIsBoolean() const {
  return (_type == Internals::JsonVariantType::JSON_BOOLEAN ||
        ((_type == Internals::JsonVariantType::JSON_UNPARSED) &&
         _content.asString &&
         ((0 == strcmp("true", _content.asString)) ||
          (0 == strcmp("false", _content.asString)))));
}

inline bool JsonVariant::variantIsInteger() const {
  using Internals::JsonVariantType;

  return _type == JsonVariantType::JSON_POSITIVE_INTEGER ||
         _type == JsonVariantType::JSON_NEGATIVE_INTEGER ||
         (_type == JsonVariantType::JSON_UNPARSED &&
          _content.asString &&
          Internals::isInteger(_content.asString));
}

inline bool JsonVariant::variantIsFloat() const {
  using Internals::JsonVariantType;

  return _type == JsonVariantType::JSON_FLOAT ||
         _type == JsonVariantType::JSON_POSITIVE_INTEGER ||
         _type == JsonVariantType::JSON_NEGATIVE_INTEGER ||
         (_type == JsonVariantType::JSON_UNPARSED &&
          _content.asString &&
          Internals::isFloat(_content.asString));
}

inline bool JsonVariant::success() const {
  switch (_type) {
    case Internals::JsonVariantType::JSON_ARRAY:
      return _content.asArray->success();

    case Internals::JsonVariantType::JSON_OBJECT:
      return _content.asObject->success();

    default:
      break;
  }

  return !variantIsUndefined();
}

#if ARDUINOJSON_ENABLE_STD_STREAM
inline std::ostream &operator<<(std::ostream &os, const JsonVariant &source) {
  return source.printTo(os);
}
#endif

}  // namespace ArduinoJson
