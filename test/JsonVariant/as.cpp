// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

using ArduinoJson::Internals::JsonInteger;
using ArduinoJson::Internals::JsonUnsignedInteger;

TEST_CASE("JsonVariant::as()") {
  SECTION("UndefinedAsBool") {
    JsonVariant variant;
    REQUIRE_FALSE(variant.as<bool>());
  }

  SECTION("UndefinedAsCstr") {
    JsonVariant variant;
    REQUIRE_FALSE(variant.as<const char*>());
  }

  SECTION("UndefinedAsString") {
    JsonVariant variant;
    REQUIRE(std::string("null") == variant.as<std::string>());
  }

  SECTION("UndefinedAsLong") {
    JsonVariant variant;
    REQUIRE(0 == variant.as<long>());
  }

  SECTION("UndefinedAsFloat") {
    JsonVariant variant;
    REQUIRE(0.0f == variant.as<float>());
  }

  SECTION("UndefinedAsDouble") {
    JsonVariant variant;
    REQUIRE(0.0 == variant.as<double>());
  }

  SECTION("DoubleAsBool") {
    JsonVariant variant = 4.2;
    REQUIRE(variant.as<bool>());
  }

  SECTION("DoubleAsCstr") {
    JsonVariant variant = 4.2;
    REQUIRE_FALSE(variant.as<const char*>());
  }

  SECTION("DoubleAsString") {
    JsonVariant variant = 4.2;
    REQUIRE(std::string("4.2") == variant.as<std::string>());
  }

  SECTION("DoubleAsLong") {
    JsonVariant variant = 4.2;
    REQUIRE(4L == variant.as<long>());
  }

  SECTION("DoubleAsUnsigned") {
    JsonVariant variant = 4.2;
    REQUIRE(4U == variant.as<unsigned>());
  }

  SECTION("DoubleZeroAsBool") {
    JsonVariant variant = 0.0;
    REQUIRE_FALSE(variant.as<bool>());
  }

  SECTION("DoubleZeroAsLong") {
    JsonVariant variant = 0.0;
    REQUIRE(0L == variant.as<long>());
  }

  SECTION("FalseAsBool") {
    JsonVariant variant = false;
    REQUIRE_FALSE(variant.as<bool>());
  }

  SECTION("FalseAsDouble") {
    JsonVariant variant = false;
    REQUIRE(0.0 == variant.as<double>());
  }

  SECTION("FalseAsLong") {
    JsonVariant variant = false;
    REQUIRE(0L == variant.as<long>());
  }

  SECTION("FalseAsString") {
    JsonVariant variant = false;
    REQUIRE(std::string("false") == variant.as<std::string>());
  }

  SECTION("TrueAsBool") {
    JsonVariant variant = true;
    REQUIRE(variant.as<bool>());
  }

  SECTION("TrueAsDouble") {
    JsonVariant variant = true;
    REQUIRE(1.0 == variant.as<double>());
  }

  SECTION("TrueAsLong") {
    JsonVariant variant = true;
    REQUIRE(1L == variant.as<long>());
  }

  SECTION("TrueAsString") {
    JsonVariant variant = true;
    REQUIRE(std::string("true") == variant.as<std::string>());
  }

  SECTION("LongAsBool") {
    JsonVariant variant = 42L;
    REQUIRE(variant.as<bool>());
  }

  SECTION("LongZeroAsBool") {
    JsonVariant variant = 0L;
    REQUIRE_FALSE(variant.as<bool>());
  }

  SECTION("PositiveLongAsDouble") {
    JsonVariant variant = 42L;
    REQUIRE(42.0 == variant.as<double>());
  }

  SECTION("NegativeLongAsDouble") {
    JsonVariant variant = -42L;
    REQUIRE(-42.0 == variant.as<double>());
  }

  SECTION("LongAsString") {
    JsonVariant variant = 42L;
    REQUIRE(std::string("42") == variant.as<std::string>());
  }

  SECTION("LongZeroAsDouble") {
    JsonVariant variant = 0L;
    REQUIRE(0.0 == variant.as<double>());
  }

  SECTION("UndefinedAsJsonNumber") {
    JsonVariant variant = JsonUndefined{};
    CHECK_FALSE(variant.as<JsonNumber>());
  }

  SECTION("NullAsJsonNumber") {
    JsonVariant variant = JsonNull{};
    CHECK_FALSE(variant.as<JsonNumber>());
  }

  SECTION("FalseAsJsonNumber") {
    JsonVariant variant = false;
    JsonNumber number = variant.as<JsonNumber>();
    CHECK(number);

    const auto convert = number.convertTo<JsonUnsignedInteger>();
    CHECK(convert);
    REQUIRE(convert.value == 0);
  }

  SECTION("TrueAsJsonNumber") {
    JsonVariant variant = true;
    JsonNumber number = variant.as<JsonNumber>();
    CHECK(number);

    const auto convert = number.convertTo<JsonUnsignedInteger>();
    CHECK(convert);
    REQUIRE(convert.value == 1);
  }

  SECTION("SignedIntegerAsJsonNumber") {
    JsonVariant variant = -42;
    JsonNumber number = variant.as<JsonNumber>();
    CHECK(number);

    const auto convert = number.convertTo<JsonInteger>();
    CHECK(convert);
    REQUIRE(convert.value == -42);
  }

  SECTION("UnsignedIntegerAsJsonNumber") {
    JsonVariant variant = 42;
    JsonNumber number = variant.as<JsonNumber>();
    CHECK(number);

    const auto convert = number.convertTo<JsonUnsignedInteger>();
    CHECK(convert);
    REQUIRE(convert.value == 42);
  }

  SECTION("FloatAsJsonNumber") {
    JsonVariant variant = 3.14;
    JsonNumber number = variant.as<JsonNumber>();
    CHECK(number);

    const auto asFloat = number.convertTo<float>();
    CHECK(asFloat);
    REQUIRE(asFloat.value == Approx(3.14f));

    const auto asDouble = number.convertTo<double>();
    CHECK(asDouble);
    REQUIRE(asDouble.value == Approx(3.14));
  }

  SECTION("StringAsJsonNumber") {
    JsonVariant variant = "hello world";
    CHECK_FALSE(variant.as<JsonNumber>());
  }

  SECTION("NumberStringAsJsonNumber") {
    JsonVariant variant = "3.14";
    JsonNumber number = variant.as<JsonNumber>();
    CHECK(number);

    const auto asFloat = number.convertTo<float>();
    CHECK(asFloat);
    REQUIRE(asFloat.value == Approx(3.14f));

    const auto asDouble = number.convertTo<double>();
    CHECK(asDouble);
    REQUIRE(asDouble.value == Approx(3.14));

    const auto asSigned = number.convertTo<JsonInteger>();
    CHECK(asSigned);
    REQUIRE(asSigned.value == 3);

    const auto asUnsigned = number.convertTo<JsonUnsignedInteger>();
    CHECK(asUnsigned);
    REQUIRE(asUnsigned.value == 3);
  }

  SECTION("NullAsBool") {
    JsonVariant variant = JsonNull{};
    REQUIRE_FALSE(variant.as<bool>());
  }

  SECTION("NullAsDouble") {
    JsonVariant variant = JsonNull{};
    REQUIRE(0.0 == variant.as<double>());
  }

  SECTION("NullAsLong") {
    JsonVariant variant = JsonNull{};
    REQUIRE(0L == variant.as<long>());
  }

  SECTION("NullAsString") {
    JsonVariant variant = JsonNull{};
    REQUIRE(std::string("null") == variant.as<std::string>());
  }

  SECTION("NumberZeroStringAsBool") {
    JsonVariant variant = "0";
    REQUIRE(variant.as<bool>());
  }

  SECTION("NumberStringAsBool") {
    JsonVariant variant = "42";
    REQUIRE(variant.as<bool>());
  }

  SECTION("NumberZeroRawStringAsBool") {
    JsonVariant variant = RawJson("0");
    REQUIRE(true == variant.as<bool>());
  }

  SECTION("NumberZeroRawStringAsBool") {
    JsonVariant variant = RawJson("42");
    REQUIRE(true == variant.as<bool>());
  }

  SECTION("NumberStringAsLong") {
    JsonVariant variant = "42";
    REQUIRE(42L == variant.as<long>());
  }

  SECTION("NumberRawStringAsLong") {
    JsonVariant variant = RawJson("42");
    REQUIRE(42L == variant.as<long>());
  }

#if ARDUINOJSON_USE_LONG_LONG
  SECTION("NumberStringAsLongLongMin") {
    JsonVariant variant = "-9223372036854775808";
    REQUIRE(std::numeric_limits<long long>::min() ==
            variant.as<long long>());
  }

  SECTION("NumberRawStringAsLongLongMin") {
    JsonVariant variant = RawJson("-9223372036854775808");
    REQUIRE(std::numeric_limits<long long>::min() ==
            variant.as<long long>());
  }

  SECTION("NumberStringAsLongLongMax") {
    JsonVariant variant = "9223372036854775807";
    REQUIRE(std::numeric_limits<long long>::max() ==
            variant.as<long long>());
  }

  SECTION("NumberRawStringAsLongLongMax") {
    JsonVariant variant = RawJson("9223372036854775807");
    REQUIRE(std::numeric_limits<long long>::max() ==
            variant.as<long long>());
  }

  SECTION("NumberStringAsUnsignedLongLongMin") {
    JsonVariant variant = "0";
    REQUIRE(std::numeric_limits<unsigned long long>::min() ==
            variant.as<unsigned long long>());
  }

  SECTION("NumberRawStringAsUnsignedLongLongMin") {
    JsonVariant variant = RawJson("0");
    REQUIRE(std::numeric_limits<unsigned long long>::min() ==
            variant.as<unsigned long long>());
  }

  SECTION("NumberStringAsUnsignedLongLongMax") {
    JsonVariant variant = "18446744073709551615";
    REQUIRE(std::numeric_limits<unsigned long long>::max() ==
            variant.as<unsigned long long>());
  }

  SECTION("NumberRawStringAsUnsignedLongLongMax") {
    JsonVariant variant = RawJson("18446744073709551615");
    REQUIRE(std::numeric_limits<unsigned long long>::max() ==
            variant.as<unsigned long long>());
  }
#endif

  SECTION("TrueStringAsBool") {
    JsonVariant variant = "true";
    REQUIRE(variant.as<bool>());
  }

  SECTION("TrueRawStringAsBool") {
    JsonVariant variant = RawJson("true");
    REQUIRE(variant.as<bool>());
  }

  SECTION("FalseStringAsBool") {
    JsonVariant variant = "false";
    REQUIRE_FALSE(variant.as<bool>());
  }

  SECTION("FalseRawStringAsBool") {
    JsonVariant variant = RawJson("false");
    REQUIRE_FALSE(variant.as<bool>());
  }

  SECTION("RandomStringAsBool") {
    JsonVariant variant = "hello";
    REQUIRE(variant.as<bool>());
  }

  SECTION("RandomStringAsLong") {
    JsonVariant variant = "hello";
    REQUIRE(0L == variant.as<long>());
  }

  SECTION("RandomStringAsConstCharPtr") {
    JsonVariant variant = "hello";
    REQUIRE(std::string("hello") == variant.as<const char*>());
  }

  SECTION("RandomStringAsCharPtr") {
    JsonVariant variant = "hello";
    REQUIRE(std::string("hello") == variant.as<char*>());
  }

  SECTION("RandomStringAsString") {
    JsonVariant variant = "hello";
    REQUIRE(std::string("hello") == variant.as<std::string>());
  }

  SECTION("NullptrStringAsCharPtr") {
    JsonVariant variant = static_cast<const char *>(nullptr);
    REQUIRE(nullptr == variant.as<const char*>());
    REQUIRE(nullptr == variant.as<char*>());
  }

  SECTION("NullptrStringAsString") {
    JsonVariant variant = static_cast<const char *>(nullptr);
    REQUIRE(std::string("") == variant.as<std::string>());
  }

  SECTION("UnparsedStringAsConstCharPtr") {
    JsonVariant variant = RawJson("nulle");
    REQUIRE(std::string("nulle") == variant.as<const char*>());
    REQUIRE(std::string("nulle") == variant.as<char*>());
  }

  SECTION("BufferAllocated") {
    DynamicJsonBuffer jb;

    SECTION("ObjectAsString") {
      JsonObject& obj = jb.createObject();
      obj["key"] = "value";

      JsonVariant variant = obj;
      REQUIRE(std::string("{\"key\":\"value\"}") == variant.as<std::string>());
    }

    SECTION("ArrayAsString") {
      JsonArray& arr = jb.createArray();
      arr.add(4);
      arr.add(2);

      JsonVariant variant = arr;
      REQUIRE(std::string("[4,2]") == variant.as<std::string>());
    }

    // as<...>() for reference-only types always resolves as T&

    SECTION("ArrayAsJsonArray") {
      JsonArray& arr = jb.createArray();

      JsonVariant variant = arr;
      REQUIRE(std::addressof(arr) ==
              std::addressof(variant.as<const JsonArray&>()));
      REQUIRE(std::addressof(arr) ==
              std::addressof(variant.as<JsonArray&>()));
      REQUIRE(std::addressof(arr) ==
              std::addressof(variant.as<const JsonArray>()));
      REQUIRE(std::addressof(arr) ==
              std::addressof(variant.as<JsonArray>()));
    }

    SECTION("ObjectAsJsonObject") {
      JsonObject& obj = jb.createObject();

      JsonVariant variant = obj;
      REQUIRE(std::addressof(obj) ==
              std::addressof(variant.as<const JsonObject&>()));
      REQUIRE(std::addressof(obj) ==
              std::addressof(variant.as<JsonObject&>()));
      REQUIRE(std::addressof(obj) ==
              std::addressof(variant.as<const JsonObject>()));
      REQUIRE(std::addressof(obj) ==
              std::addressof(variant.as<JsonObject>()));
    }
  }
}
