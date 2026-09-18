// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

using ArduinoJson::Internals::JsonString;
using StringBufferValue =
  ArduinoJson::Internals::JsonVariantContent::StringBufferValue;

namespace {

struct Check {
  bool is_undefined{};

  bool is_null{};

  bool is_bool{};

  bool is_array{};
  bool is_object{};

  bool is_number{};

  bool is_float{};
  bool is_signed{};
  bool is_unsigned{};

  bool is_string{};
};

// ref. Numbers/parseNumber.ipp
// limit the number of repeating CHECK(...) to limit the number of unique catch-generated assert object instances
#pragma push_macro("CATCH_INTERNAL_LINEINFO")
#undef CATCH_INTERNAL_LINEINFO
#define CATCH_INTERNAL_LINEINFO source_line_info

#define __CATCH_LOCAL_LINEINFO ::Catch::SourceLineInfo source_line_info = ::Catch::SourceLineInfo(__builtin_FILE(), static_cast<size_t>(__builtin_LINE()))
#define __CATCH_NESTED_LINEINFO ::Catch::SourceLineInfo source_line_info

void checkVariant(JsonVariant variant, Check check, __CATCH_LOCAL_LINEINFO) {
  CHECK(variant.is<JsonUndefined>() == check.is_undefined);
  CHECK(variant.is<JsonNull>() == check.is_null);
  CHECK(variant.is<bool>() == check.is_bool);

  CHECK(variant.is<const JsonArray>() == check.is_array);
  CHECK(variant.is<JsonArray>() == check.is_array);
  CHECK(variant.is<const JsonArray&>() == check.is_array);
  CHECK(variant.is<JsonArray&>() == check.is_array);

  CHECK(variant.is<const JsonObject>() == check.is_object);
  CHECK(variant.is<JsonObject>() == check.is_object);
  CHECK(variant.is<const JsonObject&>() == check.is_object);
  CHECK(variant.is<JsonObject&>() == check.is_object);

  CHECK(variant.is<JsonNumber>() == check.is_number);

  CHECK(variant.is<float>() == check.is_float);
  CHECK(variant.is<double>() == check.is_float);

  CHECK(variant.is<int8_t>() == check.is_signed);
  CHECK(variant.is<int16_t>() == check.is_signed);
  CHECK(variant.is<int32_t>() == check.is_signed);
  CHECK(variant.is<int64_t>() == check.is_signed);

  CHECK(variant.is<uint8_t>() == check.is_unsigned);
  CHECK(variant.is<uint16_t>() == check.is_unsigned);
  CHECK(variant.is<uint32_t>() == check.is_unsigned);
  CHECK(variant.is<uint64_t>() == check.is_unsigned);

  CHECK(variant.is<const char*>() == check.is_string);
  CHECK(variant.is<std::string>() == check.is_string);
}

#pragma pop_macro("CATCH_INTERNAL_LINEINFO")

}

TEST_CASE("JsonVariant::is()") {
  SECTION("undefined") {
    JsonVariant variant;

    Check check{};
    check.is_undefined = true;

    checkVariant(variant, check);
  }

  SECTION("null") {
    JsonVariant variant = JsonNull{};

    Check check{};
    check.is_null = true;

    checkVariant(variant, check);
  }

  SECTION("bool") {
    JsonVariant variant = false;

    Check check{};
    check.is_bool = true;

    checkVariant(variant, check);
  }

  SECTION("JsonArray") {
    DynamicJsonBuffer jb;
    JsonVariant variant = jb.createArray();

    Check check{};
    check.is_array = true;

    checkVariant(variant, check);
  }

  SECTION("JsonObject") {
    DynamicJsonBuffer jb;
    JsonVariant variant = jb.createObject();

    Check check{};
    check.is_object = true;

    checkVariant(variant, check);
  }

  SECTION("float") {
    JsonVariant variant = 4.2f;

    Check check{};
    check.is_number = true;
    check.is_float = true;

    checkVariant(variant, check);
  }

#if ARDUINOJSON_USE_DOUBLE
  SECTION("double") {
    JsonVariant variant = 4.2;

    Check check{};
    check.is_number = true;
    check.is_float = true;

    checkVariant(variant, check);
  }
#endif

  SECTION("int") {
    JsonVariant variant = 42;

    Check check{};
    check.is_number = true;
    check.is_float = true;
    check.is_signed = true;
    check.is_unsigned = true;

    checkVariant(variant, check);
  }

  SECTION("string") {
    JsonVariant variant = "42";

    Check check{};
    check.is_string = true;

    checkVariant(variant, check);
  }

  SECTION("string buffer") {
    JsonVariant variant = StringBufferValue{"42"};

    Check check{};
    check.is_string = true;

    checkVariant(variant, check);
  }

  SECTION("parsed string") {
    auto variant = JsonVariant(JsonString("42"), true);

    Check check{};
    check.is_string = true;

    checkVariant(variant, check);

    variant = JsonVariant(JsonString(StringBufferValue{"42"}), true);
    checkVariant(variant, check);

    variant = JsonVariant(JsonString("wat"), false);
    checkVariant(variant, check);

    variant = JsonVariant(JsonString(StringBufferValue{"wat"}), false);
    checkVariant(variant, check);
  }

  SECTION("unparsed null") {
    JsonVariant variant = RawJson("null");

    Check check{};
    check.is_null = true;
    check.is_string = true;

    checkVariant(variant, check);
  }

  SECTION("unparsed bool") {
    JsonVariant variant = RawJson("false");

    Check check{};
    check.is_bool = true;
    check.is_string = true;

    checkVariant(variant, check);

    variant = RawJson("true");
    checkVariant(variant, check);
  }

  SECTION("unparsed int") {
    JsonVariant variant = RawJson("42");

    Check check{};
    check.is_number = true;
    check.is_float = true;
    check.is_signed = true;
    check.is_unsigned = true;
    check.is_string = true;

    checkVariant(variant, check);
  }

  SECTION("unparsed float") {
    JsonVariant variant = RawJson("42e-10");

    Check check{};
    check.is_number = true;
    check.is_float = true;
    check.is_string = true;

    checkVariant(variant, check);
  }
}
