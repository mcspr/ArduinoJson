// MIT License

#define ARDUINOJSON_ENABLE_UTF8_BOM 1
#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonBuffer::parse() w/ BOM parsing enabled") {
  DynamicJsonBuffer jb;

  SECTION("Empty string") {
    JsonVariant variant = jb.parse("");
    REQUIRE_FALSE(variant.success());
  }

  SECTION("Just BOM") {
    JsonVariant variant = jb.parse("\xef\xbb\xbf");
    REQUIRE_FALSE(variant.success());
  }

  SECTION("Null") {
    JsonVariant variant = jb.parse("\xef\xbb\xbf" "null");
    REQUIRE(variant.success());
    REQUIRE(variant.is<JsonNull>());
  }

  SECTION("False") {
    JsonVariant variant = jb.parse("\xef\xbb\xbf" "false");
    REQUIRE(variant.success());
    REQUIRE(variant.is<bool>());
    REQUIRE(variant.as<bool>() == false);
    REQUIRE(variant == false);
  }

  SECTION("True") {
    JsonVariant variant = jb.parse("\xef\xbb\xbf" "true");
    REQUIRE(variant.success());
    REQUIRE(variant.is<bool>());
    REQUIRE(variant.as<bool>() == true);
    REQUIRE(variant == true);
  }

  SECTION("Integer") {
    JsonVariant variant = jb.parse("\xef\xbb\xbf" "-42");
    REQUIRE(variant.success());
    REQUIRE(variant.is<int>());
    REQUIRE_FALSE(variant.is<bool>());
    REQUIRE(variant == -42);
  }

  SECTION("Double") {
    JsonVariant variant = jb.parse("\xef\xbb\xbf" "-1.23e+4");
    REQUIRE(variant.success());
    REQUIRE_FALSE(variant.is<int>());
    REQUIRE(variant.is<double>());
    REQUIRE(variant.as<double>() == Approx(-1.23e+4));
  }

  SECTION("Unquoted string") {
    std::string testCase = "\xef\xbb\xbf" "12345";
    JsonVariant variant = jb.parse(testCase);
    REQUIRE(variant.success());
    REQUIRE(variant == std::string("12345"));
  }

  SECTION("Double quoted string") {
    SECTION("Complete byte sequence") {
      std::string testCase = "\xef\xbb\xbf" "\"12345\"";
      JsonVariant variant = jb.parse(testCase);
      REQUIRE(variant.success());
      REQUIRE(variant == std::string("12345"));
    }

    SECTION("One byte short") {
      std::string testCase = "\xef\xbb" "\"12345\"";
      JsonVariant variant = jb.parse(testCase);
      REQUIRE_FALSE(variant.success());
    }

    SECTION("Two bytes short") {
      std::string testCase = "\xef" "\"12345\"";
      JsonVariant variant = jb.parse(testCase);
      REQUIRE_FALSE(variant.success());
    }

    SECTION("BOM before and after the string") {
      std::string testCase = "\xef\xbb\xbf\"12345\"\xef\xbb\xbf";
      JsonVariant variant = jb.parse(testCase);
      REQUIRE(variant.success());
      REQUIRE(variant == std::string("12345"));
    }

    SECTION("BOM after the string") {
      std::string testCase = "\"12345\"\xef\xbb\xbf";
      JsonVariant variant = jb.parse(testCase);
      REQUIRE(variant.success());
      REQUIRE(variant == std::string("12345"));
    }
  }

  SECTION("Single quoted string") {
    SECTION("Complete byte sequence") {
      std::string testCase = "\xef\xbb\xbf" "'12345'";
      JsonVariant variant = jb.parse(testCase);
      REQUIRE(variant.success());
      REQUIRE(variant == std::string("12345"));
    }

    SECTION("One byte short") {
      std::string testCase = "\xef\xbb" "'12345'";
      JsonVariant variant = jb.parse(testCase);
      REQUIRE_FALSE(variant.success());
    }

    SECTION("Two bytes short") {
      std::string testCase = "\xef" "'12345'";
      JsonVariant variant = jb.parse(testCase);
      REQUIRE_FALSE(variant.success());
    }

    SECTION("BOM before and after the string") {
      std::string testCase = "\xef\xbb\xbf'12345'\xef\xbb\xbf";
      JsonVariant variant = jb.parse(testCase);
      REQUIRE(variant.success());
      REQUIRE(variant == std::string("12345"));
    }

    SECTION("BOM after the string") {
      std::string testCase = "'12345'\xef\xbb\xbf";
      JsonVariant variant = jb.parse(testCase);
      REQUIRE(variant.success());
      REQUIRE(variant == std::string("12345"));
    }
  }

  SECTION("Object") {
    SECTION("BOM before the braces") {
      JsonVariant variant = jb.parse("\xef\xbb\xbf" "{}");
      REQUIRE(variant.success());
      REQUIRE(variant.is<JsonObject>());
    }

    SECTION("BOM before and incide the braces") {
      JsonVariant variant = jb.parse("\xef\xbb\xbf" "{" "\xef\xbb\xbf" "}");
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<JsonObject>());
    }

    SECTION("BOM inside the braces") {
      JsonVariant variant = jb.parse("{" "\xef\xbb\xbf" "}");
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<JsonObject>());
    }

    SECTION("BOM before the key") {
      JsonVariant variant = jb.parse("{" "\xef\xbb\xbf" "key: " "value" "}");
      REQUIRE_FALSE(variant.success());
    }

    SECTION("BOM before braces and before the key") {
      JsonVariant variant = jb.parse("\xef\xbb\xbf" "{" "\xef\xbb\xbf" "key: " "value" "}");
      REQUIRE_FALSE(variant.success());
    }

    SECTION("BOM after the key") {
      JsonVariant variant = jb.parse("{" "\"key\"" "\xef\xbb\xbf" ": " "value" "}");
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<JsonObject>());
    }

    SECTION("BOM before braces and after the key") {
      JsonVariant variant = jb.parse("\xef\xbb\xbf" "{" "\"key\"" "\xef\xbb\xbf" ": " "value" "}");
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<JsonObject>());
    }

    SECTION("BOM before the value") {
      JsonVariant variant = jb.parse("{" "key: " "\xef\xbb\xbf" "\"value\"" "}");
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<JsonObject>());
    }

    SECTION("BOM before the braces and before the value") {
      JsonVariant variant = jb.parse("\xef\xbb\xbf" "{" "key: " "\xef\xbb\xbf" "\"value\"" "}");
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<JsonObject>());
    }

    SECTION("BOM after the value") {
      JsonVariant variant = jb.parse("{" "key: " "\"value\"" "\xef\xbb\xbf" "}");
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<JsonObject>());
    }

    SECTION("BOM before the braces and after the value") {
      JsonVariant variant = jb.parse("\xef\xbb\xbf" "{" "key: " "\"value\"" "\xef\xbb\xbf" "}");
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<JsonObject>());
    }

    SECTION("BOM after the brace w/ value") {
      JsonVariant variant = jb.parse("{ key: value }" "\xef\xbb\xbf");
      REQUIRE(variant.success());
      REQUIRE(variant.is<JsonObject>());
      REQUIRE(variant.as<JsonObject&>()["key"] == "value");
    }

    SECTION("BOM before and after the object") {
      JsonVariant variant = jb.parse("\xef\xbb\xbf" "{ key: value }" "\xef\xbb\xbf");
      REQUIRE(variant.success());
      REQUIRE(variant.is<JsonObject>());
      REQUIRE(variant.as<JsonObject&>()["key"] == "value");
    }

    SECTION("BOM after the object") {
      JsonVariant variant = jb.parse("{}" "\xef\xbb\xbf");
      REQUIRE(variant.success());
      REQUIRE(variant.is<JsonObject>());
    }
  }

  SECTION("Array") {
    SECTION("BOM before the brackets") {
      JsonVariant variant = jb.parse("\xef\xbb\xbf" "[]");
      REQUIRE(variant.success());
      REQUIRE(variant.is<JsonArray>());
    }

    SECTION("BOM before and incide the brackets") {
      JsonVariant variant = jb.parse("\xef\xbb\xbf" "[" "\xef\xbb\xbf" "]");
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<JsonArray>());
    }

    SECTION("BOM inside the brackets") {
      JsonVariant variant = jb.parse("[" "\xef\xbb\xbf" "]");
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<JsonArray>());
    }

    SECTION("BOM before the value") {
      JsonVariant variant = jb.parse("[" "\xef\xbb\xbf" "\"value\"" "]");
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<JsonArray>());
    }

    SECTION("BOM before the brackets and before the value") {
      JsonVariant variant = jb.parse("\xef\xbb\xbf" "[" "\xef\xbb\xbf" "\"value\"" "]");
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<JsonArray>());
    }

    SECTION("BOM after the value") {
      JsonVariant variant = jb.parse("[" "\"value\"" "\xef\xbb\xbf" "]");
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<JsonArray>());
    }

    SECTION("BOM before the brackets and after the value") {
      JsonVariant variant = jb.parse("\xef\xbb\xbf" "[ \"value\"" "\xef\xbb\xbf" "]");
      REQUIRE_FALSE(variant.success());
      REQUIRE_FALSE(variant.is<JsonArray>());
    }

    SECTION("BOM after the bracket w/ value") {
      JsonVariant variant = jb.parse("[ value ]" "\xef\xbb\xbf");
      REQUIRE(variant.success());
      REQUIRE(variant.is<JsonArray>());
      REQUIRE(variant.as<JsonArray&>()[0] == "value");
    }

    SECTION("BOM before and after the array") {
      JsonVariant variant = jb.parse("\xef\xbb\xbf" "[ value ]" "\xef\xbb\xbf");
      REQUIRE(variant.success());
      REQUIRE(variant.is<JsonArray>());
      REQUIRE(variant.as<JsonArray&>()[0] == "value");
    }

    SECTION("BOM after the array") {
      JsonVariant variant = jb.parse("[]" "\xef\xbb\xbf");
      REQUIRE(variant.success());
      REQUIRE(variant.is<JsonArray>());
    }
  }
}
