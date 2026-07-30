// MIT License

#pragma once

#include "../Data/JsonVariantContent.hpp"
#include "../TypeTraits/RemoveReference.hpp"
#include "../JsonString.hpp"

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

// Wrap TJsonBuffer::startString() to delay allocating until JsonVariantContent string buffer fills up
// Usually only happens when parsing 'const char*', keeping small keys from appearing in the buffer
template <typename TJsonBuffer>
struct StringBufferedWriter {
  using buffer_type = JsonVariantContent::StringBufferValue;
  using string_type = typename RemoveReference<TJsonBuffer>::type::String;

  explicit StringBufferedWriter(TJsonBuffer& jsonBuffer) :
    _jsonBuffer(jsonBuffer)
  {}

  struct String {
    explicit String(TJsonBuffer& parent) :
      _buffer(makeBuffer()),
      _parentString(parent.startString())
    {}

    JsonString asJsonString() {
      if (isInternalBuffer())
        return JsonString(_buffer);

      return JsonString(_parentString.c_str());
    }

    void append(char c) {
      (this->*_append)(_parentString, c);
    }

   private:
    const char* data() const {
      return &_buffer.value[0];
    }

    buffer_type buffer() const {
      return _buffer;
    }

    char& last() {
      return _buffer.value[LastIndex];
    }

    char last() const {
      return _buffer.value[LastIndex];
    }

    char& operator[](size_t index) {
      return _buffer.value[index];
    }

    char operator[](size_t index) const {
      return _buffer.value[index];
    }

    void _appendValue(string_type& str, char c);
    void _appendParent(string_type& str, char c);

    bool isInternalBuffer() const {
      return _append == &String::_appendValue;
    }

    bool isParentBuffer() const {
      return _append == &String::_appendParent;
    }

    using append_type = void(String::*)(string_type&, char);
    append_type _append{ &String::_appendValue };

    buffer_type _buffer;
    string_type _parentString;
  };

  String startString() const {
    return String(_jsonBuffer);
  }

 private:
  static constexpr auto LastIndex = size_t{ sizeof(buffer_type) - 1 };

  static buffer_type makeBuffer() {
    JsonVariantContent::StringBufferValue out{};
    out.value[LastIndex] = static_cast<char>(LastIndex);
    return out;
  }

  TJsonBuffer& _jsonBuffer;
};

}
}
