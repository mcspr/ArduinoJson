// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonVariant undefined") {
  JsonVariant variant;

  SECTION("AsCharReturns0") {
    REQUIRE(char{0} == variant.as<char>());
  }

  SECTION("AsShortReturns0") {
    REQUIRE(short{0} == variant.as<short>());
  }

  SECTION("AsIntReturns0") {
    REQUIRE(int{0} == variant.as<int>());
  }

  SECTION("AsLongReturns0") {
    REQUIRE(0L == variant.as<long>());
  }

#if ARDUINOJSON_USE_LONG_LONG
  SECTION("AsLongLongReturns0") {
    REQUIRE(0LL == variant.as<long long>());
  }
#endif

  SECTION("AsUnsignedCharReturns0") {
    REQUIRE(0 == variant.as<unsigned char>());
  }

  SECTION("AsUnsignedShortReturns0") {
    REQUIRE(0 == variant.as<unsigned short>());
  }

  SECTION("AsUnsignedIntReturns0") {
    REQUIRE(0 == variant.as<unsigned int>());
  }

  SECTION("AsUnsignedLongReturns0") {
    REQUIRE(0UL == variant.as<unsigned long>());
  }

#if ARDUINOJSON_USE_LONG_LONG
  SECTION("AsUnsignedLongLongReturns0") {
    REQUIRE(0ULL == variant.as<unsigned long long>());
  }
#endif

  SECTION("AsFloatReturns0") {
    REQUIRE(Approx(0.0f) == variant.as<float>());
  }

#if ARDUINOJSON_USE_DOUBLE
  SECTION("AsDoubleReturns0") {
    REQUIRE(Approx(0.0) == variant.as<double>());
  }
#endif

  SECTION("AsBoolReturnsFalse") {
    REQUIRE(false == variant.as<bool>());
  }

  SECTION("AsCstringReturnsNullptr") {
    REQUIRE(nullptr == variant.as<const char*>());
  }

  SECTION("AsStdStringReturnsNullString") {
    REQUIRE(std::string("null") == variant.as<std::string>());
  }

  SECTION("AsArrayReturnInvalid") {
    REQUIRE(JsonArray::invalid() == variant.as<JsonArray&>());
  }

  SECTION("AsConstArrayReturnInvalid") {
    REQUIRE(JsonArray::invalid() == variant.as<const JsonArray&>());
  }

  SECTION("AsObjectReturnInvalid") {
    REQUIRE(JsonObject::invalid() == variant.as<JsonObject&>());
  }

  SECTION("AsConstObjectReturnInvalid") {
    REQUIRE(JsonObject::invalid() == variant.as<const JsonObject&>());
  }

  SECTION("AsArrayWrapperReturnInvalid") {
    REQUIRE(JsonArray::invalid() == variant.as<JsonArray>());
  }

  SECTION("AsConstArrayWrapperReturnInvalid") {
    REQUIRE(JsonArray::invalid() == variant.as<const JsonArray>());
  }

  SECTION("AsObjectWrapperReturnInvalid") {
    REQUIRE(JsonObject::invalid() == variant.as<JsonObject>());
  }

  SECTION("AsConstObjectWrapperReturnInvalid") {
    REQUIRE(JsonObject::invalid() == variant.as<const JsonObject>());
  }
}
