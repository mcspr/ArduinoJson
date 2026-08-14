// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonArray::copyTo()") {
  DynamicJsonBuffer jsonBuffer;

  SECTION("BiggerOneDimensionIntegerArray") {
    char json[] = "[1,2,3]";
    const JsonArray& src = jsonBuffer.parseArray(json);

    int dst[4]{};
    dst[3] = 12345;

    size_t result = src.copyTo(dst);

    REQUIRE(3 == result);
    REQUIRE(src[0] == dst[0]);
    REQUIRE(src[1] == dst[1]);
    REQUIRE(src[2] == dst[2]);
    REQUIRE(12345 == dst[3]);
  }

  SECTION("SmallerOneDimensionIntegerArray") {
    char json[] = "[1,2,3]";
    const JsonArray& src = jsonBuffer.parseArray(json);

    int dst[2];
    size_t result = src.copyTo(dst);

    REQUIRE(2 == result);
    REQUIRE(src[0] == dst[0]);
    REQUIRE(src[1] == dst[1]);
  }

  SECTION("TwoOneDimensionIntegerArray") {
    char json[] = "[[1,2],[3],[4],[]]";

    const JsonArray& src = jsonBuffer.parseArray(json);

    int dst[4][2]{};
    dst[1][1] = 12345;
    dst[2][1] = 67890;
    dst[3][0] = 11111;
    dst[3][1] = 22222;
    src.copyTo(dst);

    REQUIRE(src[0][0] == dst[0][0]);
    REQUIRE(src[0][1] == dst[0][1]);
    REQUIRE(src[1][0] == dst[1][0]);
    REQUIRE(12345 == dst[1][1]);
    REQUIRE(src[2][0] == dst[2][0]);
    REQUIRE(67890 == dst[2][1]);
    REQUIRE(11111 == dst[3][0]);
    REQUIRE(22222 == dst[3][1]);
  }
}
