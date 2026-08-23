// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson/Serialization/StaticStringBuilder.hpp>
#include <ArduinoJson/Serialization/DynamicStringBuilder.hpp>

#include <catch.hpp>

#include <array>
#include <algorithm>
#include <string>

// while the static version consumes and returns as 'written' the same amount of bytes as the dynamic,
// actual output is size + 1 b/c after every char or string write there is an additional '\0' written
using ArduinoJson::Internals::StaticStringBuilder;
using ArduinoJson::Internals::DynamicStringBuilder;

template <typename StringBuilder, typename String>
void common_tests(StringBuilder& sb, const String& output) {
  SECTION("InitialState") {
    REQUIRE(std::string("") == output);
  }

  SECTION("EmptyString") {
    REQUIRE(0 == sb.print(""));
    REQUIRE(std::string("") == output);
  }

  SECTION("OneString") {
    REQUIRE(4 == sb.print("ABCD"));
    REQUIRE(std::string("ABCD") == output);
  }

  SECTION("TwoStrings") {
    REQUIRE(4 == sb.print("ABCD"));
    REQUIRE(4 == sb.print("EFGH"));
    REQUIRE(std::string("ABCDEFGH") == output);
  }
}

TEST_CASE("StaticStringBuilder") {
  std::string tmp;

  std::array<char, 20> output;
  output.fill('*');
  output[0] = '\0';

  StaticStringBuilder sb(output.data(), output.size());
  common_tests(sb, static_cast<const char*>(output.data()));

  SECTION("BelowCapacity") {
    std::string smaller_than_output = "ABCDE";
    REQUIRE(output.size() - 1 > smaller_than_output.size());

    REQUIRE(smaller_than_output.size() == sb.print(smaller_than_output.data()));
    REQUIRE(smaller_than_output == std::string(output.data()));

    auto it = output.data() + smaller_than_output.size();
    REQUIRE(*(it++) == '\0');
    REQUIRE(std::all_of(it, output.end(), [](char c) { return c == '*'; }));
  }

  SECTION("ExactCapacity") {
    std::string strings[] = {
      "ABCDEFGHIJ",
      "12345",
      "678",
    };

    size_t size{};
    for (const auto& s : strings)
      size += s.size();

    REQUIRE(size == output.size() - 2);

    auto it = output.data();
    for (const auto& s : strings) {
      REQUIRE(s.size() == sb.print(s.data()));

      tmp += s;
      REQUIRE(tmp == std::string(output.data()));

      it += s.size();
      REQUIRE(it != output.end());
      REQUIRE(*it == '\0');
      REQUIRE(std::all_of((it + 1), output.end(), [](char c) { return c == '*'; }));
    }

    std::string larger_than_remaining = "FEFEFE";
    REQUIRE(1 == sb.print(larger_than_remaining.data()));

    it += 1;
    REQUIRE(*(it++) == '\0');
    REQUIRE(it == output.end());

    tmp.append(larger_than_remaining.data(), 1);
    REQUIRE(tmp == std::string(output.data()));

    REQUIRE(0 == sb.print("12345"));
  }

  SECTION("OverCapacity") {
    static constexpr size_t capacity = output.size() - 1;
    std::string larger_than_output = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    REQUIRE(capacity < larger_than_output.size());
    REQUIRE(capacity == sb.print(larger_than_output.data()));

    std::string larger_than_remaining = "12345";
    REQUIRE(0 == sb.print(larger_than_remaining.data()));

    tmp.append(larger_than_output.data(), capacity);
    REQUIRE(tmp == std::string(output.data()));
  }
}

TEST_CASE("DynamicStringBuilder") {
  std::string output;
  DynamicStringBuilder<std::string> sb(output);
  common_tests(sb, output);
}
