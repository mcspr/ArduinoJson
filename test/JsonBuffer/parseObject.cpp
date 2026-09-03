// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonBuffer::parseObject()") {
  DynamicJsonBuffer jb;

  SECTION("An empty object") {
    JsonObject& obj = jb.parseObject("{}");
    REQUIRE(obj.success());
    REQUIRE(obj.size() == 0);
  }

  SECTION("An empty quoted key") {
    JsonObject& obj = jb.parseObject("{\"\": \"value\"}");
    REQUIRE(obj.success());
    REQUIRE(obj.size() == 1);
    REQUIRE(obj[""] == "value");
  }

  SECTION("An empty key without quotes") {
    JsonObject& obj = jb.parseObject("{: \"value\"}");
    REQUIRE_FALSE(obj.success());
  }

  SECTION("Null byte in key") {
    JsonObject& obj = jb.parseObject("{\"123\0\": \"value\"}");
    REQUIRE_FALSE(obj.success());
  }

  SECTION("Null byte in value") {
    JsonObject& obj = jb.parseObject("{\"key\": \"va\0lue\"}");
    REQUIRE_FALSE(obj.success());
  }

  SECTION("Null byte somewhere") {
    JsonObject& obj = jb.parseObject("{\"key\":\0\"value\"}");
    REQUIRE_FALSE(obj.success());
  }

  SECTION("An incomplete key quotes") {
    REQUIRE_FALSE(jb.parseObject("{\"key:\"value\"}").success());
    REQUIRE_FALSE(jb.parseObject("{key\":\"value\"}").success());
    REQUIRE_FALSE(jb.parseObject("{'key:\"value\"}").success());
    REQUIRE_FALSE(jb.parseObject("{key':\"value\"}").success());
    REQUIRE_FALSE(jb.parseObject("{\"key':\"value\"}").success());
    REQUIRE_FALSE(jb.parseObject("{'key\":\"value\"}").success());
  }

  SECTION("Quotes") {
    SECTION("Double quotes") {
      JsonObject& obj = jb.parseObject("{\"key\":\"value\"}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 1);
      REQUIRE(obj["key"] == "value");
    }

    SECTION("Single quotes") {
      JsonObject& obj = jb.parseObject("{'key':'value'}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 1);
      REQUIRE(obj["key"] == "value");
    }

    SECTION("Mixed quotes") {
      JsonObject& obj = jb.parseObject("{'key1':\"value1\",\"key2\":'value2'}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 2);
      REQUIRE(obj["key1"] == "value1");
      REQUIRE(obj["key2"] == "value2");
    }

    SECTION("No quotes") {
      JsonObject& obj = jb.parseObject("{key:value}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 1);
      REQUIRE(obj["key"] == "value");
    }

    SECTION("No quotes, allow underscore in key") {
      JsonObject& obj = jb.parseObject("{_k_e_y_:42}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 1);
      REQUIRE(obj["_k_e_y_"] == 42);
    }

    SECTION("No quotes, key without value") {
      JsonObject& obj = jb.parseObject("{key}");
      REQUIRE_FALSE(obj.success());
    }

    SECTION("No quotes, spaced out key without value") {
      JsonObject& obj = jb.parseObject("{ key }");
      REQUIRE_FALSE(obj.success());
    }

    SECTION("No quotes, brace after key") {
      JsonObject& obj = jb.parseObject("{key}:value}");
      REQUIRE_FALSE(obj.success());
    }

    SECTION("No quotes, brace after colon") {
      JsonObject& obj = jb.parseObject("{key:}value}");
      REQUIRE_FALSE(obj.success());
    }
  }

  SECTION("Spaces") {
    SECTION("Before the key") {
      JsonObject& obj = jb.parseObject("{ \"key\":\"value\"}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 1);
      REQUIRE(obj["key"] == "value");
    }

    SECTION("After the key") {
      JsonObject& obj = jb.parseObject("{\"key\" :\"value\"}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 1);
      REQUIRE(obj["key"] == "value");
    }

    SECTION("Before the value") {
      JsonObject& obj = jb.parseObject("{\"key\": \"value\"}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 1);
      REQUIRE(obj["key"] == "value");
    }

    SECTION("After the value") {
      JsonObject& obj = jb.parseObject("{\"key\":\"value\" }");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 1);
      REQUIRE(obj["key"] == "value");
    }

    SECTION("Before the colon") {
      JsonObject& obj =
          jb.parseObject("{\"key1\":\"value1\" ,\"key2\":\"value2\"}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 2);
      REQUIRE(obj["key1"] == "value1");
      REQUIRE(obj["key2"] == "value2");
    }

    SECTION("After the colon") {
      JsonObject& obj =
          jb.parseObject("{\"key1\":\"value1\" ,\"key2\":\"value2\"}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 2);
      REQUIRE(obj["key1"] == "value1");
      REQUIRE(obj["key2"] == "value2");
    }
  }

  SECTION("Values types") {
    SECTION("String") {
      JsonObject& obj =
          jb.parseObject("{\"key1\":\"value1\",\"key2\":\"value2\"}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 2);
      REQUIRE(obj["key1"] == "value1");
      REQUIRE(obj["key2"] == "value2");
    }

    SECTION("Integer") {
      JsonObject& obj = jb.parseObject("{\"key1\":42,\"key2\":-42}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 2);
      REQUIRE(obj["key1"] == 42);
      REQUIRE(obj["key2"] == -42);
    }

    SECTION("Double") {
      JsonObject& obj = jb.parseObject("{\"key1\":12.345,\"key2\":-7E89}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 2);
      REQUIRE(obj["key1"] == Approx(12.345));
      REQUIRE(obj["key2"] == -7E89);
    }

    SECTION("Boolean") {
      JsonObject& obj = jb.parseObject("{\"key1\":true,\"key2\":false}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 2);
      REQUIRE(obj["key1"] == true);
      REQUIRE(obj["key2"] == false);
    }

    SECTION("Null") {
      JsonObject& obj = jb.parseObject("{\"key1\":null,\"key2\":null}");
      REQUIRE(obj.success());
      REQUIRE(obj.size() == 2);
      REQUIRE(obj["key1"].as<const char*>() == std::string("null"));
      REQUIRE(obj["key2"].as<const char*>() == std::string("null"));
    }
  }

  SECTION("Misc") {
    SECTION("The opening brace is missing") {
      JsonObject& obj = jb.parseObject("}");
      REQUIRE_FALSE(obj.success());
    }

    SECTION("The closing brace is missing") {
      JsonObject& obj = jb.parseObject("{");
      REQUIRE_FALSE(obj.success());
    }

    SECTION("A quoted key without value") {
      JsonObject& obj = jb.parseObject("{\"key\"}");
      REQUIRE_FALSE(obj.success());
    }

    SECTION("A non-quoted key without value") {
      JsonObject& obj = jb.parseObject("{key}");
      REQUIRE_FALSE(obj.success());
    }

    SECTION("A dangling comma") {
      JsonObject& obj = jb.parseObject("{\"key1\":\"value1\",}");
      REQUIRE_FALSE(obj.success());
      REQUIRE(obj.size() == 0);
    }

    SECTION("null as a key") {
      JsonObject& obj = jb.parseObject("null:\"value\"}");
      REQUIRE_FALSE(obj.success());
    }
  }

  SECTION("Trailing comments") {
    SECTION("CCommentAfterClosingBrace") {
      JsonObject& obj = jb.parseObject("{\"hello\": \"world\"  }/*COMMENT*/");

      REQUIRE(obj.success());
      REQUIRE(1 == obj.size());
      REQUIRE(obj["hello"] == "world");
    }

    SECTION("CppCommentAfterClosingBrace") {
      JsonObject& obj = jb.parseObject("{\"hello\":\"world\"}//COMMENT\n");

      REQUIRE(obj.success());
      REQUIRE(1 == obj.size());
      REQUIRE(obj["hello"] == "world");
    }
  }
}
