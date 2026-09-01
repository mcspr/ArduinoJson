// MIT License

#pragma once

#include "../Configuration.hpp"
#include "../TypeTraits/Constant.hpp"

#include <cstdint>

namespace ArduinoJson {
namespace Internals {
namespace JsonParserImpl {

template <typename NestingLimit, typename EnableComments, typename SkipBom>
struct DefaultDeserializationOptions {
  static constexpr uint8_t nestingLimit() {
    return NestingLimit::value;
  }

  static constexpr bool enableComments() {
    return EnableComments::value;
  }

  static constexpr bool skipBom() {
    return SkipBom::value;
  }
};

}

using DefaultDeserializationOptions =
  JsonParserImpl::DefaultDeserializationOptions<
    IntegralConstant<uint8_t, ARDUINOJSON_DEFAULT_NESTING_LIMIT>,
    BooleanConstant<ARDUINOJSON_ENABLE_COMMENTS == 1>,
    BooleanConstant<ARDUINOJSON_ENABLE_UTF8_BOM == 1>>;

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

template <typename TOptions>
constexpr DeserializationOptions makeDefaultDeserializationOptions(TOptions options) {
  return DeserializationOptions(
    options.nestingLimit(), options.enableComments(), options.skipBom());
}

}
}
