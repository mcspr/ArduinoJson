// MIT License

#pragma once

#include "../Configuration.hpp"

#include <cstdint>

namespace ArduinoJson {
namespace Internals {

struct DeserializationOptions {
  constexpr DeserializationOptions(uint8_t nestingLimit_, bool enableComments_, bool skipBom_) noexcept :
    nestingLimit(nestingLimit_),
    enableComments(enableComments_),
    skipBom(skipBom_)
  {}

  constexpr DeserializationOptions(uint8_t nestingLimit_, bool enableComments_) noexcept :
    nestingLimit(nestingLimit_),
    enableComments(enableComments_)
  {}

  constexpr DeserializationOptions(uint8_t nestingLimit_) noexcept :
    nestingLimit(nestingLimit_)
  {}

  constexpr DeserializationOptions() = default;

  uint8_t nestingLimit{ ARDUINOJSON_DEFAULT_NESTING_LIMIT };
  bool enableComments{ ARDUINOJSON_ENABLE_COMMENTS == 1};
  bool skipBom{ ARDUINOJSON_ENABLE_UTF8_BOM == 1};
};

}
}
