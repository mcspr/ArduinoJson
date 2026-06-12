// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson/Serialization/DynamicStringBuilder.hpp>
#include <ArduinoJson/Serialization/JsonWriter.hpp>

#include <cmath>
#include <limits>
#include <string>

#include <catch.hpp>

using ArduinoJson::Internals::DynamicStringBuilder;
using ArduinoJson::Internals::JsonWriter;

template <typename TFloat>
void check(TFloat input, const std::string& expected) {
  std::string output;
  DynamicStringBuilder<std::string> sb(output);
  JsonWriter<DynamicStringBuilder<std::string> > writer(sb);
  writer.writeFloat(input);
  REQUIRE(writer.bytesWritten() == output.size());
  CHECK(expected == output);
}

TEST_CASE("JsonWriter::writeFloat(double)") {
  SECTION("Pi") {
    check<double>(std::acos(-1.0), "3.141592654");
  }

  SECTION("Signaling NaN") {
    auto nan = std::numeric_limits<double>::signaling_NaN();
    check<double>(nan, "NaN");
  }

  SECTION("Quiet NaN") {
    auto nan = std::numeric_limits<double>::quiet_NaN();
    check<double>(nan, "NaN");
  }

  SECTION("Infinity") {
    auto inf = std::numeric_limits<double>::infinity();
    check<double>(inf, "Infinity");
    check<double>(-inf, "-Infinity");
  }

  SECTION("Zero") {
    check<double>(0.0, "0");
    check<double>(-0.0, "-0");
  }

  SECTION("Epsilon") {
    auto epsilon = std::numeric_limits<double>::epsilon();
    check<double>(epsilon, "2.220446049e-16");
    check<double>(-epsilon, "-2.220446049e-16");
  }

  SECTION("Min double") {
    auto minval = std::numeric_limits<double>::min();
    check<double>(minval, "2.225073859e-308");
    check<double>(-minval, "-2.225073859e-308");
  }

  SECTION("Max double") {
    auto maxval = std::numeric_limits<double>::max();
    check<double>(maxval, "1.797693135e308");
    check<double>(-maxval, "-1.797693135e308");
  }

  SECTION("Big exponent") {
    // this test increases coverage of normalize()
    check<double>(1e255, "1e255");
    check<double>(1e-255, "1e-255");
  }

  SECTION("Exponentation when <= 1e-5") {
    check<double>(1e-4, "0.0001");
    check<double>(1e-5, "1e-5");

    check<double>(-1e-4, "-0.0001");
    check<double>(-1e-5, "-1e-5");
  }

  SECTION("Exponentation when >= 1e7") {
    check<double>(9999999.999, "9999999.999");
    check<double>(10000000.0, "1e7");

    check<double>(-9999999.999, "-9999999.999");
    check<double>(-10000000.0, "-1e7");
  }

  SECTION("Rounding when too many decimals") {
    check<double>(0.000099999999999, "0.0001");
    check<double>(0.0000099999999999, "1e-5");
    check<double>(0.9999999996, "1");
  }

  SECTION("9 decimal places") {
    check<double>(0.100000001, "0.100000001");
    check<double>(0.999999999, "0.999999999");

    check<double>(9.000000001, "9.000000001");
    check<double>(9.999999999, "9.999999999");
  }

  SECTION("10 decimal places") {
    check<double>(0.1000000001, "0.1");
    check<double>(0.9999999999, "1");

    check<double>(9.0000000001, "9");
    check<double>(9.9999999999, "10");
  }
}

TEST_CASE("JsonWriter::writeFloat(float)") {
  SECTION("Pi") {
    check<float>(std::acos(-1.0f), "3.141593");
  }

  SECTION("Signaling NaN") {
    auto nan = std::numeric_limits<float>::signaling_NaN();
    check<float>(nan, "NaN");
  }

  SECTION("Quiet NaN") {
    auto nan = std::numeric_limits<float>::quiet_NaN();
    check<float>(nan, "NaN");
  }

  SECTION("Infinity") {
    auto inf = std::numeric_limits<float>::infinity();
    check<float>(inf, "Infinity");
    check<float>(-inf, "-Infinity");
  }

  SECTION("Zero") {
    check<float>(0.0f, "0");
    check<float>(-0.0f, "-0");
  }

  SECTION("Epsilon") {
    auto epsilon = std::numeric_limits<float>::epsilon();
    check<float>(epsilon, "1.192093e-7");
    check<float>(-epsilon, "-1.192093e-7");
  }

  SECTION("Min float") {
    auto minval = std::numeric_limits<float>::min();
    check<float>(minval, "1.175494e-38");
    check<float>(-minval, "-1.175494e-38");
  }

  SECTION("Max float") {
    auto maxval = std::numeric_limits<float>::max();
    check<float>(maxval, "3.402823e38");
    check<float>(-maxval, "-3.402823e38");
  }

  SECTION("999.9") {  // issue #543
    check<float>(999.9f, "999.9");
  }

  SECTION("24.3") {  // # issue #588
    check<float>(24.3f, "24.3");
  }
}
