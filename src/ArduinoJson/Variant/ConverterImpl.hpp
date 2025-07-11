// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Json/JsonSerializer.hpp>
#include <ArduinoJson/Memory/StringBuilder.hpp>
#include <ArduinoJson/Polyfills/utility.hpp>
#include <ArduinoJson/Variant/JsonVariantConst.hpp>

#if ARDUINOJSON_ENABLE_STD_STRING
#  include <string>
#endif

#if ARDUINOJSON_ENABLE_STRING_VIEW
#  include <string_view>
#endif

ARDUINOJSON_BEGIN_PUBLIC_NAMESPACE

template <typename T, typename Enable>
struct Converter {
  static_assert(!detail::is_same<T, char>::value,
                "type 'char' is not supported, use 'signed char', 'unsigned "
                "char' or another integer type instead");

  static void toJson(const T& src, JsonVariant dst) {
    // clang-format off
    convertToJson(src, dst); // Error here? See https://arduinojson.org/v7/unsupported-set/
    // clang-format on
  }

  static detail::decay_t<T> fromJson(JsonVariantConst src) {
    static_assert(!detail::is_same<T, char*>::value,
                  "type 'char*' is not supported, use 'const char*' instead");

    // clang-format off
    T result; // Error here? See https://arduinojson.org/v7/non-default-constructible/
    convertFromJson(src, result);  // Error here? See https://arduinojson.org/v7/unsupported-as/
    // clang-format on
    return result;
  }

  static bool checkJson(JsonVariantConst src) {
    static_assert(!detail::is_same<T, char*>::value,
                  "type 'char*' is not supported, use 'const char*' instead");

    T dummy = T();
    // clang-format off
    return canConvertFromJson(src, dummy);  // Error here? See https://arduinojson.org/v7/unsupported-is/
    // clang-format on
  }
};

template <typename T>
struct Converter<T, detail::enable_if_t<detail::is_integral<T>::value &&
                                        !detail::is_same<bool, T>::value &&
                                        !detail::is_same<char, T>::value>>
    : private detail::VariantAttorney {
  static bool toJson(T src, JsonVariant dst) {
    ARDUINOJSON_ASSERT_INTEGER_TYPE_IS_SUPPORTED(T);
    auto variant = getImpl(dst);
    variant.clear();
    return variant.setInteger(src);
  }

  static T fromJson(JsonVariantConst src) {
    ARDUINOJSON_ASSERT_INTEGER_TYPE_IS_SUPPORTED(T);
    return getImpl(src).template asIntegral<T>();
  }

  static bool checkJson(JsonVariantConst src) {
    return getImpl(src).template isInteger<T>();
  }
};

template <typename T>
struct Converter<T, detail::enable_if_t<detail::is_enum<T>::value>>
    : private detail::VariantAttorney {
  static bool toJson(T src, JsonVariant dst) {
    return dst.set(static_cast<JsonInteger>(src));
  }

  static T fromJson(JsonVariantConst src) {
    return static_cast<T>(getImpl(src).template asIntegral<int>());
  }

  static bool checkJson(JsonVariantConst src) {
    return getImpl(src).template isInteger<int>();
  }
};

template <>
struct Converter<bool> : private detail::VariantAttorney {
  static bool toJson(bool src, JsonVariant dst) {
    auto impl = getImpl(dst);
    impl.clear();
    return impl.setBoolean(src);
  }

  static bool fromJson(JsonVariantConst src) {
    return getImpl(src).asBoolean();
  }

  static bool checkJson(JsonVariantConst src) {
    return getImpl(src).type() == detail::VariantType::Boolean;
  }
};

template <typename T>
struct Converter<T, detail::enable_if_t<detail::is_floating_point<T>::value>>
    : private detail::VariantAttorney {
  static bool toJson(T src, JsonVariant dst) {
    auto impl = getImpl(dst);
    impl.clear();
    return impl.setFloat(src);
  }

  static T fromJson(JsonVariantConst src) {
    return getImpl(src).template asFloat<T>();
  }

  static bool checkJson(JsonVariantConst src) {
    return getImpl(src).isFloat();
  }
};

template <>
struct Converter<const char*> : private detail::VariantAttorney {
  static void toJson(const char* src, JsonVariant dst) {
    auto impl = getImpl(dst);
    impl.clear();
    impl.setString(detail::adaptString(src));
  }

  static const char* fromJson(JsonVariantConst src) {
    return getImpl(src).asString().c_str();
  }

  static bool checkJson(JsonVariantConst src) {
    return getImpl(src).isString();
  }
};

template <>
struct Converter<JsonString> : private detail::VariantAttorney {
  static void toJson(JsonString src, JsonVariant dst) {
    auto impl = getImpl(dst);
    impl.clear();
    impl.setString(detail::adaptString(src));
  }

  static JsonString fromJson(JsonVariantConst src) {
    return getImpl(src).asString();
  }

  static bool checkJson(JsonVariantConst src) {
    return getImpl(src).isString();
  }
};

template <typename T>
inline detail::enable_if_t<detail::IsString<T>::value> convertToJson(
    const T& src, JsonVariant dst) {
  auto variant = detail::VariantAttorney::getImpl(dst);
  variant.clear();
  variant.setString(detail::adaptString(src));
}

// SerializedValue<std::string>
// SerializedValue<String>
// SerializedValue<const __FlashStringHelper*>
template <typename T>
struct Converter<SerializedValue<T>> : private detail::VariantAttorney {
  static void toJson(SerializedValue<T> src, JsonVariant dst) {
    auto variant = getImpl(dst);
    variant.clear();
    variant.setRawString(src);
  }
};

template <>
struct Converter<detail::nullptr_t> : private detail::VariantAttorney {
  static void toJson(detail::nullptr_t, JsonVariant dst) {
    getImpl(dst).clear();
  }
  static detail::nullptr_t fromJson(JsonVariantConst) {
    return nullptr;
  }
  static bool checkJson(JsonVariantConst src) {
    return getImpl(src).isNull();
  }
};

#if ARDUINOJSON_ENABLE_ARDUINO_STREAM

namespace detail {
class StringBuilderPrint : public Print {
 public:
  StringBuilderPrint(ResourceManager* resources) : copier_(resources) {
    copier_.startString();
  }

  void save(VariantData* data) {
    ARDUINOJSON_ASSERT(!overflowed());
    copier_.save(data);
  }

  size_t write(uint8_t c) {
    copier_.append(char(c));
    return copier_.isValid() ? 1 : 0;
  }

  size_t write(const uint8_t* buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
      copier_.append(char(buffer[i]));
      if (!copier_.isValid())
        return i;
    }
    return size;
  }

  bool overflowed() const {
    return !copier_.isValid();
  }

 private:
  StringBuilder copier_;
};
}  // namespace detail

inline void convertToJson(const ::Printable& src, JsonVariant dst) {
  auto impl = detail::VariantAttorney::getImpl(dst);
  if (impl.isUnbound())
    return;
  impl.clear();
  detail::StringBuilderPrint print(impl.resources());
  src.printTo(print);
  if (print.overflowed())
    return;
  print.save(impl.data());
}

#endif

#if ARDUINOJSON_ENABLE_ARDUINO_STRING

inline void convertFromJson(JsonVariantConst src, ::String& dst) {
  JsonString str = src.as<JsonString>();
  if (str)
    dst = str.c_str();
  else
    serializeJson(src, dst);
}

inline bool canConvertFromJson(JsonVariantConst src, const ::String&) {
  return src.is<JsonString>();
}

#endif

#if ARDUINOJSON_ENABLE_STD_STRING

inline void convertFromJson(JsonVariantConst src, std::string& dst) {
  JsonString str = src.as<JsonString>();
  if (str)
    dst.assign(str.c_str(), str.size());
  else
    serializeJson(src, dst);
}

inline bool canConvertFromJson(JsonVariantConst src, const std::string&) {
  return src.is<JsonString>();
}

#endif

#if ARDUINOJSON_ENABLE_STRING_VIEW

inline void convertFromJson(JsonVariantConst src, std::string_view& dst) {
  JsonString str = src.as<JsonString>();
  if (str)  // the standard doesn't allow passing null to the constructor
    dst = std::string_view(str.c_str(), str.size());
}

inline bool canConvertFromJson(JsonVariantConst src, const std::string_view&) {
  return src.is<JsonString>();
}

#endif

template <>
struct Converter<JsonArrayConst> : private detail::VariantAttorney {
  static void toJson(JsonArrayConst src, JsonVariant dst) {
    if (src.isNull())
      dst.set(nullptr);
    else
      dst.to<JsonArray>().set(src);
  }

  static JsonArrayConst fromJson(JsonVariantConst src) {
    return JsonArrayConst(getImpl(src));
  }

  static bool checkJson(JsonVariantConst src) {
    return getImpl(src).type() == detail::VariantType::Array;
  }
};

template <>
struct Converter<JsonArray> : private detail::VariantAttorney {
  static void toJson(JsonVariantConst src, JsonVariant dst) {
    if (src.isNull())
      dst.set(nullptr);
    else
      dst.to<JsonArray>().set(src);
  }

  static JsonArray fromJson(JsonVariant src) {
    return JsonArray(getImpl(src));
  }

  static bool checkJson(JsonVariant src) {
    return getImpl(src).type() == detail::VariantType::Array;
  }
};

template <>
struct Converter<JsonObjectConst> : private detail::VariantAttorney {
  static void toJson(JsonVariantConst src, JsonVariant dst) {
    if (src.isNull())
      dst.set(nullptr);
    else
      dst.to<JsonObject>().set(src);
  }

  static JsonObjectConst fromJson(JsonVariantConst src) {
    return JsonObjectConst(getImpl(src));
  }

  static bool checkJson(JsonVariantConst src) {
    return getImpl(src).type() == detail::VariantType::Object;
  }
};

template <>
struct Converter<JsonObject> : private detail::VariantAttorney {
  static void toJson(JsonVariantConst src, JsonVariant dst) {
    if (src.isNull())
      dst.set(nullptr);
    else
      dst.to<JsonObject>().set(src);
  }

  static JsonObject fromJson(JsonVariant src) {
    return JsonObject(getImpl(src));
  }

  static bool checkJson(JsonVariant src) {
    return getImpl(src).type() == detail::VariantType::Object;
  }
};

ARDUINOJSON_END_PUBLIC_NAMESPACE
