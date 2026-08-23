// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <catch.hpp>

#include <ArduinoJson/Serialization/JsonWriter.hpp>
#include <ArduinoJson/Serialization/StaticStringBuilder.hpp>

using ArduinoJson::Internals::JsonWriter;
using ArduinoJson::Internals::JsonWriterSink;
using ArduinoJson::Internals::StaticStringBuilder;

void check(const char* input, std::string expected) {
  char output[1024];
  StaticStringBuilder sb(output, sizeof(output));
  JsonWriterSink<StaticStringBuilder> sink(sb);
  JsonWriter writer(&sink);
  writer.writeString(input);
  REQUIRE(expected == output);
  REQUIRE(writer.bytesWritten() == expected.size());
}

TEST_CASE("JsonWriter::writeString()") {
  SECTION("Nullptr") {
    check(nullptr, "\"\"");
  }

  SECTION("EmptyString") {
    check("", "\"\"");
  }

  SECTION("QuotationMark") {
    check("\"", "\"\\\"\"");
  }

  SECTION("ReverseSolidus") {
    check("\\", "\"\\\\\"");
  }

  SECTION("Solidus") {
    check("/", "\"/\"");  // but the JSON format allows \/
  }

  SECTION("StartofHeading") {
    check("\x1", "\"\\u0001\"");
  }

  SECTION("StartOfText") {
    check("\x2", "\"\\u0002\"");
  }

  SECTION("EndOfText") {
    check("\x3", "\"\\u0003\"");
  }

  SECTION("EndOfTransmission") {
    check("\x4", "\"\\u0004\"");
  }

  SECTION("Enquiry") {
    check("\x5", "\"\\u0005\"");
  }

  SECTION("Acknowledge") {
    check("\x6", "\"\\u0006\"");
  }

  SECTION("Alert") {
    check("\x7", "\"\\u0007\"");
  }

  SECTION("Backspace") {
    check("\b", "\"\\b\"");
  }

  SECTION("HorizontalTabulation") {
    check("\t", "\"\\t\"");
  }

  SECTION("Newline") {
    check("\n", "\"\\n\"");
  }

  SECTION("VerticalTabulation") {
    check("\xb", "\"\\u000b\"");
  }

  SECTION("Formfeed") {
    check("\f", "\"\\f\"");
  }

  SECTION("CarriageReturn") {
    check("\r", "\"\\r\"");
  }

  SECTION("ShiftOut") {
    check("\xe", "\"\\u000e\"");
  }

  SECTION("ShiftIn") {
    check("\xf", "\"\\u000f\"");
  }

  SECTION("DataLinkEscape") {
    check("\x10", "\"\\u0010\"");
  }

  SECTION("DeviceControlOne") {
    check("\x11", "\"\\u0011\"");
  }

  SECTION("DeviceControlTwo") {
    check("\x12", "\"\\u0012\"");
  }

  SECTION("DeviceControlThree") {
    check("\x13", "\"\\u0013\"");
  }

  SECTION("DeviceControlFour") {
    check("\x14", "\"\\u0014\"");
  }

  SECTION("NegativeAcknowledge") {
    check("\x15", "\"\\u0015\"");
  }

  SECTION("SynchronousIdle") {
    check("\x16", "\"\\u0016\"");
  }

  SECTION("EndOfTransmission") {
    check("\x17", "\"\\u0017\"");
  }

  SECTION("Cancel") {
    check("\x18", "\"\\u0018\"");
  }

  SECTION("EndOfMedium") {
    check("\x19", "\"\\u0019\"");
  }

  SECTION("Substitute") {
    check("\x1a", "\"\\u001a\"");
  }

  SECTION("Escape") {
    check("\x1b", "\"\\u001b\"");
  }

  SECTION("FileSeparator") {
    check("\x1c", "\"\\u001c\"");
  }

  SECTION("GroupSeparator") {
    check("\x1d", "\"\\u001d\"");
  }

  SECTION("RecordSeparator") {
    check("\x1e", "\"\\u001e\"");
  }

  SECTION("UnitSeparator") {
    check("\x1f", "\"\\u001f\"");
  }

  SECTION("Space") {
    check("\x20", "\" \"");
  }
}
