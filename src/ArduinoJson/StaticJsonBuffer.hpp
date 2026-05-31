// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonBufferBase.hpp"

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

class StaticJsonBufferBase : public JsonBufferBase<StaticJsonBufferBase> {
 public:
  class String {
   public:
    String(StaticJsonBufferBase* parent) :
      _parent(parent),
      _start(parent->_buffer + parent->_size)
    {}

    void append(char c) {
      if (_parent->canAlloc(1)) {
        char* last = static_cast<char*>(_parent->doAlloc(1));
        *last = c;
      }
    }

    const char* c_str() const {
      if (_parent->canAlloc(1)) {
        char* last = static_cast<char*>(_parent->doAlloc(1));
        *last = '\0';
        return _start;
      } else {
        return nullptr;
      }
    }

   private:
    StaticJsonBufferBase* _parent;
    char* _start;
  };

  StaticJsonBufferBase(char* buffer, size_t capa)
      : _buffer(buffer), _capacity(capa), _size(0) {}

  // Gets the capacity of the buffer in bytes
  size_t capacity() const {
    return _capacity;
  }

  // Gets the current usage of the buffer in bytes
  size_t size() const {
    return _size;
  }

  // Allocates the specified amount of bytes in the buffer
  void* alloc(size_t bytes) override {
    alignNextAlloc();
    if (canAlloc(bytes))
      return doAlloc(bytes);

    return nullptr;
  }

  // Resets the buffer.
  // USE WITH CAUTION: this invalidates all previously allocated data
  void clear() {
    _size = 0;
  }

  String startString() {
    return String(this);
  }

 protected:
  ~StaticJsonBufferBase() {}

 private:
  void alignNextAlloc() {
    _size = round_size_up(_size);
  }

  bool canAlloc(size_t bytes) const {
    return _size + bytes <= _capacity;
  }

  void* doAlloc(size_t bytes) {
    void* p = &_buffer[_size];
    _size += bytes;
    return p;
  }

  char* _buffer;
  size_t _capacity;
  size_t _size;
};
}  // namespace Internals

// Implements a JsonBuffer with fixed memory allocation.
// The template paramenter CAPACITY specifies the capacity of the buffer in
// bytes.
template <size_t CAPACITY>
class StaticJsonBuffer final : public Internals::StaticJsonBufferBase {
 public:
  explicit StaticJsonBuffer()
      : Internals::StaticJsonBufferBase(_buffer, CAPACITY) {}

  ~StaticJsonBuffer() = default;

 private:
  char _buffer[CAPACITY];
};
}  // namespace ArduinoJson
