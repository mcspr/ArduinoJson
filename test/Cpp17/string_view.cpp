// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

#include <string_view>

struct ConstructStringView {
  static std::string_view Operator(const char* str = nullptr) {
    if (str)
      return std::string_view(str, std::strlen(str));

    return std::string_view();
  }
};

template <>
struct ArduinoJson::Internals::StringTraitsImpl<std::string_view> :
  ArduinoJson::Internals::StringTraitsTag {

  using Construct = ConstructStringView;
};

TEST_CASE("string_view") {
  SECTION("JsonVariant::as()") {
    JsonVariant variant;

    SECTION("Empty") {
      auto view = variant.as<std::string_view>();
      REQUIRE(view.empty());
    }

    SECTION("Null") {
      variant = JsonNull{};
      auto view = variant.as<std::string_view>();
      REQUIRE(view.empty());
    }

    SECTION("Boolean") {
      variant = JsonNull{};
      auto view = variant.as<std::string_view>();
      REQUIRE(view.empty());
    }

    SECTION("Number") {
      variant = 12345;
      auto view = variant.as<std::string_view>();
      REQUIRE(view.empty());
    }

    SECTION("Null string pointer") {
      variant = static_cast<const char*>(nullptr);
      auto view = variant.as<std::string_view>();
      REQUIRE(view.empty());
    }

    SECTION("String pointer") {
      variant = "12345";
      auto view = variant.as<std::string_view>();
      REQUIRE_FALSE(view.empty());
      REQUIRE(std::string(view) == std::string("12345"));
    }

    SECTION("Unparsed string pointer") {
      variant = RawJson("12345");
      auto view = variant.as<std::string_view>();
      REQUIRE_FALSE(view.empty());
      REQUIRE(std::string(view) == std::string("12345"));
    }
  }
}

