// MIT License

#pragma once

#include "Data/JsonVariantContent.hpp"
#include "Data/JsonVariantDefault.hpp"
#include "Data/JsonVariantType.hpp"

namespace ArduinoJson {

// Forward declarations
class JsonVariant;

namespace Internals {

// JsonVariant internal helper that only stores non-raw string data
// CAUTION: only used for internal parser conversions
struct JsonString {
  using StringBufferValue = JsonVariantContent::StringBufferValue;

 private:
  struct Undefined {
    JsonVariantType type;
  };

  struct Pointer {
    JsonVariantType type;
    const char* pointer;
  };

  struct StringBuffer {
    JsonVariantType type;
    StringBufferValue buffer;
  };

  union Content {
    Content() noexcept :
      undefined({JsonVariantType::JSON_UNDEFINED})
    {}

    explicit Content(const char* pointer) noexcept :
      asStringPointer({JsonVariantType::JSON_STRING, pointer})
    {}

    explicit Content(StringBufferValue buffer) noexcept :
      asStringBuffer({JsonVariantType::JSON_STRING_BUFFER, buffer})
    {}

    Undefined undefined;
    Pointer asStringPointer;
    StringBuffer asStringBuffer;
  };

  struct PointerVisitor {
    static const char* Operator(const char* pointer) {
      return pointer;
    }

    static const char* Operator(const StringBufferValue& buffer) {
      return &buffer.value[0];
    }
  };

  template <typename R, typename T>
  R visit(T&& visitor) const {
    using Internals::JsonVariantType;
  
    if (_content.asStringPointer.type == JsonVariantType::JSON_STRING) {
      return visitor.Operator(_content.asStringPointer.pointer);
  
    } else if (_content.asStringBuffer.type == JsonVariantType::JSON_STRING_BUFFER) {
      return visitor.Operator(_content.asStringBuffer.buffer);
  
    }
  
    return Internals::JsonVariantDefault<R>::get();
  }

  Content _content;
  friend class ArduinoJson::JsonVariant;

 public:
  JsonString() noexcept = default;

  explicit JsonString(const char* pointer) noexcept :
    _content(pointer)
  {}

  explicit JsonString(StringBufferValue buffer) noexcept :
    _content(buffer)
  {}

  bool success() const {
    return _content.undefined.type != JsonVariantType::JSON_UNDEFINED;
  }

  const char* c_str() const {
    return visit<const char*>(PointerVisitor{});
  }
};

}
}
