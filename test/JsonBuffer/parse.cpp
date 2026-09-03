// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

#include <cctype>

using Catch::Matchers::Equals;

enum class IsType {
  Array,
  Object,
  String,
};

CATCH_REGISTER_ENUM(IsType, IsType::Array, IsType::Object, IsType::String)

#include "escape.ipp"

using ArduinoJson::MakeStringView;

TEST_CASE("JsonBuffer::parse()") {
  DynamicJsonBuffer jb;

  SECTION("EmptyObject") {
    JsonVariant variant = jb.parse("{}");
    REQUIRE(variant.success());
    REQUIRE(variant.is<JsonObject>());
  }

  SECTION("EmptyArray") {
    JsonVariant variant = jb.parse("[]");
    REQUIRE(variant.success());
    REQUIRE(variant.is<JsonArray>());
  }

  SECTION("Null") {
    JsonVariant variant = jb.parse("null");
    REQUIRE(variant.success());
    REQUIRE(variant.is<JsonNull>());
  }

  SECTION("False") {
    JsonVariant variant = jb.parse("false");
    REQUIRE(variant.success());
    REQUIRE(variant.is<bool>());
    REQUIRE(variant.as<bool>() == false);
    REQUIRE(variant == false);
  }

  SECTION("True") {
    JsonVariant variant = jb.parse("true");
    REQUIRE(variant.success());
    REQUIRE(variant.is<bool>());
    REQUIRE(variant.as<bool>() == true);
    REQUIRE(variant == true);
  }

  SECTION("Integer") {
    JsonVariant variant = jb.parse("-42");
    REQUIRE(variant.success());
    REQUIRE(variant.is<int>());
    REQUIRE_FALSE(variant.is<bool>());
    REQUIRE(variant == -42);
  }

  SECTION("Double") {
    JsonVariant variant = jb.parse("-1.23e+4");
    REQUIRE(variant.success());
    REQUIRE_FALSE(variant.is<int>());
    REQUIRE(variant.is<double>());
    REQUIRE(variant.as<double>() == Approx(-1.23e+4));
  }

  auto testStringCase = [&](const std::string& testCase, char quote) {
      CAPTURE(escape(testCase));

      JsonVariant variant = jb.parse(testCase);
      REQUIRE(variant.success());
      REQUIRE(variant.is<const char*>());

      std::string tmp;
      if (quote != '\0')
        tmp += quote;

      tmp += variant.as<std::string>();  // deserialized value
      if (quote != '\0')
        tmp += quote;

      REQUIRE_THAT(tmp, Equals(testCase));
  };

  SECTION("Double quoted string") {
    SECTION("Plain string") {
      std::string testCases[] = {
        "\"hello world\"",
        "\"a\"",
        "\"abcde12345\"",
        "\"\x71\"",
        "\"\x75\x4c\"",
        "\"\x7f\x4c\x23\x3c\x3a\x6f\x5d\x44\x20\x70\"",
      };

      for (const auto& testCase : testCases) {
        testStringCase(testCase, '"');
      }
    }

    SECTION("Escaped quote") {
      JsonVariant variant = jb.parse("\"ab\\\"cd\"");
      REQUIRE(variant.success());
      REQUIRE(variant.is<const char*>());
      REQUIRE(variant.as<std::string>() == "ab\"cd");
    }

    SECTION("Explicit view") {
      JsonVariant variant = jb.parse(MakeStringView("\"12345\""));
      REQUIRE(variant.success());
      REQUIRE(variant.is<const char*>());
      REQUIRE(variant == "12345");
    }

    SECTION("Partial parse failure") {
      JsonVariant variant = jb.parse(MakeStringView("\"12345\"", 1));
      REQUIRE_FALSE(variant.success());
    }
  }

  SECTION("Single quoted string") {
    SECTION("Plain string") {
      std::string testCases[] = {
        "'hello world'",
        "'a'",
        "'abcde12345'",
        "'\x71'",
        "'\x75\x4c'",
        "'\x7f\x4c\x23\x3c\x3a\x6f\x5d\x44\x20\x70'",
      };

      for (const auto& testCase : testCases) {
        testStringCase(testCase, '\'');
      }
    }

    SECTION("Escaped quote") {
      JsonVariant variant = jb.parse("'ab\\\'cd'");
      REQUIRE(variant.success());
      REQUIRE(variant.is<const char*>());
      REQUIRE(variant.as<std::string>() == "ab\'cd");
    }

    SECTION("Explicit view") {
      JsonVariant variant = jb.parse(MakeStringView("'12345'"));
      REQUIRE(variant.success());
      REQUIRE(variant.is<const char*>());
      REQUIRE(variant == "12345");
    }

    SECTION("Partial parse failure") {
      JsonVariant variant = jb.parse(MakeStringView("'12345'", 1));
      REQUIRE_FALSE(variant.success());
    }
  }

  SECTION("Unquoted string") {
    std::string testCases[] = {
      "a",
      "hello",
      "12345",
      "abcde12345",
      "\x71",
      "\x75\x4c",
    };

    for (const auto& testCase : testCases) {
      testStringCase(testCase, '\0');
    }
  }

  SECTION("Partial view of unquoted string") {
    JsonVariant variant = jb.parse(MakeStringView("1234567890", 5));
    REQUIRE(variant.success());
    REQUIRE(variant.is<const char*>());
    REQUIRE(variant == "12345");
  }

  SECTION("Invalid JSON") {
    std::vector<std::string> testCases;

    auto shouldFail = [&](IsType type) {
      size_t n = 1;
      for (const auto& testCase : testCases) {
        CAPTURE(n);
        CAPTURE(type);
        CAPTURE(escape(testCase));
        ++n;

        JsonVariant variant = jb.parse(testCase);
        REQUIRE_FALSE(variant.success());

        // should not propogate invalid()
        switch (type) {
        case IsType::Array:
          REQUIRE_FALSE(variant.is<JsonArray>());
          break;

        case IsType::Object:
          REQUIRE_FALSE(variant.is<JsonObject>());
          break;

        case IsType::String:
          break;
        }

        // should never point to anything
        REQUIRE_FALSE(variant.is<const char*>());
      }
    };

    // keeping u0000 here, but note that escape sequence can only appear in quotes
    SECTION("Embedded null") {
      testCases = {
        "\x00",
        "'\x00'",
        "\"\x00\"",
        "\\u0000",
        "'\\u0000'",
        "\"\\u0000\"",
        "12345678\\u0000",
        "'12345678\\u0000'",
        "\"12345678\\u0000\"",
        "hello\\u0000world",
        "'hello\\u0000world'",
        "\"hello\\u0000world\"",
        "1234 \x00 5678",
        "'1234 \x00 5678'",
        "\"1234 \x00 5678\"",
      };

      shouldFail(IsType::String);
    }

    // control character should be escaped via \\u00xx
    SECTION("Control character") {
      testCases.reserve((3 + 1) * 2 * 0x20);

      char tmp1[] = "\".\"";
      char tmp2[] = "'.'";
      for (int i = 0; i < 0x20; ++i) {
        tmp1[1] = static_cast<char>(i);
        testCases.push_back(std::string(tmp1));
        tmp2[1] = static_cast<char>(i);
        testCases.push_back(std::string(tmp2));
      }

      shouldFail(IsType::String);
    }

    // ref. https://github.com/simdutf/is_utf8/
    // ref. https://github.com/crossbario/autobahn-testsuite
    // todo: ensure most bad_sequence stuff is ported here?
    // escaped 'u' assumes invalid utf16 sequence converted to utf8
    // raw 'x' assumes invalid utf8 sequence (escaped 'x' not supported by the spec)
    SECTION("Invalid string") {
      testCases = {
        " ",
        "a ",
        "a b c",
        "hello world",
        "'\xA0\xA1'",
        "'\\uA0A1'",
        "'\xC0\x9F'",
        "'\\uC09F'",
        "'\xE2\x28\xA1'",
        "'\\uE282\x28'",
        "'\xE2\\u8228'",
        "'\xE2\x82\x28'",
        "'\\uE282\x28'",
        "'\xE2\\u8228'",
        "'\xED\xA0\x80'",
        "'\\uEDA0\x80'",
        "'\xED\\uA080'",
        "'\xED\xA0\x81'",
        "'\\uEDA0\x81'",
        "'\xED\\uA081'",
        "'\xF0\x28\x8C\x28'",
        "'\\uF028\\u8C28'",
        "'\xF0\x28\x8C\xBC'",
        "'\\uF028\\u8CBC'",
        "'\xF0\x90\x28\xBC'",
        "'\\uF090\\u28BC'",
        "'\xF5\xFF\xFF\xFF'",
        "'\\uF5FF\\uFFFF'",
        "'\xF8\x90\x80\x80\x80'",
        "'\xF8\\u9080\\u8080'",
        "'\xC3\x28'",
        "'\\uC328'",
        "'\xc2\x7f'",
        "'\\uc27f'",
        "'\x80'",
        "'\\u0080'",
        "'\x90'",
        "'\\uc290'",
        "'\xa1'",
        "'\\u00a1'",
        "'\xb2'",
        "'\\u00b2'",
        "'\xc3'",
        "'\\uc383'",
        "'\xd4'",
        "'\\uc394'",
        "'\xdf'",
        "'\\uc394'",
        "'\xe5'",
        "'\\uc3a5'",
        "'\xef\xbf'",
        "'\xef\xbf\xbf'",
        "'\\uefbf'",
        "'\xf6\xc3\xb1'",
        "'\xf6\\uc3b1'",
        "'\\uf6c3\xb1'",
        "'\xf4\x90\x80\x80'",
        "'\xf4\\u9080\x80'",
        "'\\uf490\\u8080'",
        "'\xef\xbb\xbf\x48\65\x6c\x6c\x6f'",
        "\xef\xbb\xfa'\x62\x63\x64'",
        "'\x7f\x4c\x23\x3c\x3a\x6f\x5d\x44\x13\x70'",
        "\x7f\x4c\x23\x3c\x3a\x6f\x5d\x44\x13\x70",
      };

      shouldFail(IsType::String);
    }

    SECTION("non-HEX codeunit nibble") {
      // prefix plus quotes plus 1 character plus null byte, repeated N times
      // subtracts indices of 0..9, a..f, A..f as they wouldn't be generated
      static constexpr char prefix[] = "\\u000";
      testCases.reserve((3 + sizeof(prefix)) * (256 - (9 + 6 + 6)));

      for (int i = 0; i < 256; ++i) {
        if (!std::isxdigit(i)) {
          std::string testCase;
          testCase.reserve(3 + sizeof(prefix));
          testCase += '"';
          testCase += prefix;
          testCase += static_cast<char>(i);
          testCase += '"';
          testCases.push_back(std::move(testCase));
        }
      }

      shouldFail(IsType::String);
    }

    SECTION("Incomplete string") {
      testCases = {
        "\'hello",
        "\"hello",
        "'\\u",
        "\"\\u",
        "'\\u1",
        "\"\\u1",
        "'\\u23",
        "\"\\u23",
        "'\\u456",
        "\"\\u456",
      };

      shouldFail(IsType::String);
    }

    // more in parseArray.cpp
    SECTION("Incomplete / invalid array") {
      testCases = {
        "",
        "[",
        "[,",
        "[[],",
        "[[[}]",
        "[1234:",
        "  [}] ",
        "   [",
      };

      shouldFail(IsType::Array);
    }

    // more in parseObject.cpp
    SECTION("Incomplete object") {
      testCases = {
        "",
        "{",
        "{,",
        "{{},",
        "{a:{b:{,",
        "{\"ab\": \"123",
        "{1234: []",
        " {  ]}",
        "   {:",
      };

      shouldFail(IsType::Object);
    }
  }
}
