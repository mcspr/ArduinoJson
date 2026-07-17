// MIT License

#pragma once

namespace ArduinoJson {
namespace Internals {

struct JsonLiterals {
  static constexpr const char* False = "false";
  static constexpr const char* True = "true";
  static constexpr const char* Null = "null";

  static bool isFalse(const char*);
  static bool isTrue(const char*);
  static bool isNull(const char*);
};

}
}
