// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonBufferBase.hpp"
#include "Allocator.hpp"

#include <cstdlib>

namespace ArduinoJson {
namespace Internals {

class DynamicJsonBufferBase final :
  public JsonBufferBase<DynamicJsonBufferBase> {

  struct Block;
  struct EmptyBlock {
    Block* next;
    size_t capacity;
    size_t size;
  };
  struct Block : EmptyBlock {
    uint8_t data[1];
  };

 public:
  enum { EmptyBlockSize = sizeof(EmptyBlock) };

  DynamicJsonBufferBase() = default;

  explicit DynamicJsonBufferBase(Allocator* allocator) :
    _allocator(allocator)
  {}

  explicit DynamicJsonBufferBase(size_t initialSize) :
    _nextBlockCapacity(initialSize)
  {}

  DynamicJsonBufferBase(Allocator* allocator, size_t initialSize) :
    _allocator(allocator),
    _nextBlockCapacity(initialSize)
  {}

  ~DynamicJsonBufferBase() {
    clear();
  }

  // Gets the number of bytes occupied in the buffer
  size_t size() const {
    size_t total = 0;
    for (const Block* b = _head; b; b = b->next) total += b->size;
    return total;
  }

  // Allocates the specified amount of bytes in the buffer
  void* alloc(size_t bytes) override {
    alignNextAlloc();
    return canAllocInHead(bytes) ? allocInHead(bytes) : allocInNewBlock(bytes);
  }

  // Resets the buffer.
  // USE WITH CAUTION: this invalidates all previously allocated data
  void clear() {
    Block* currentBlock = _head;
    while (currentBlock != nullptr) {
      _nextBlockCapacity = currentBlock->capacity;
      Block* nextBlock = currentBlock->next;
      _allocator->deallocate(currentBlock);
      currentBlock = nextBlock;
    }
    _head = 0;
  }

  class String {
   public:
    String(DynamicJsonBufferBase* parent)
        : _parent(parent), _start(nullptr), _length(0) {}

    void append(char c) {
      append(&c, 1);
    }

    const char* c_str() {
      append(0);
      return _start;
    }

   private:
    void _append(char* out, const char* begin, const char* end) {
      for (auto it = begin; it != end; ++it, ++out) {
        *out = *it;
      }
    }

    void append(const char* str, size_t len) {
      if (_parent->canAllocInHead(len)) {
        char* end = static_cast<char*>(_parent->allocInHead(len));
        _append(end, str, str + len);
        if (_length == 0) _start = end;
      } else {
        char* newStart =
            static_cast<char*>(_parent->allocInNewBlock(_length + len));
        if (_start && newStart) memcpy(newStart, _start, _length);
        if (newStart)
          _append(newStart + _length, str, str + len);
        else
          len = 0;
        _start = newStart;
      }
      _length += len;
    }

    DynamicJsonBufferBase* _parent;
    char* _start;
    size_t _length;
  };

  String startString() {
    return String(this);
  }

 private:
  void alignNextAlloc() {
    if (_head) _head->size = this->round_size_up(_head->size);
  }

  bool canAllocInHead(size_t bytes) const {
    return _head != nullptr && _head->size + bytes <= _head->capacity;
  }

  void* allocInHead(size_t bytes) {
    void* p = _head->data + _head->size;
    _head->size += bytes;
    return p;
  }

  void* allocInNewBlock(size_t bytes) {
    size_t capacity = _nextBlockCapacity;
    if (bytes > capacity)
      capacity = bytes;

    if (addNewBlock(capacity)) {
      _nextBlockCapacity *= 2;
      return allocInHead(bytes);
    }

    return nullptr;
  }

  bool addNewBlock(size_t capacity) {
    size_t bytes = EmptyBlockSize + capacity;
    Block* block = static_cast<Block*>(_allocator->allocate(bytes));
    if (block != nullptr) {
      block->capacity = capacity;
      block->size = 0;
      block->next = _head;
      _head = block;
      return true;
    }

    return false;
  }

  Allocator* _allocator = DefaultAllocator::instance();
  Block* _head = nullptr;
  size_t _nextBlockCapacity = 256;
};
}  // namespace Internals

// Implements a JsonBuffer with dynamic memory allocation.
// You are strongly encouraged to consider using StaticJsonBuffer which is much
// more suitable for embedded systems.
typedef Internals::DynamicJsonBufferBase DynamicJsonBuffer;

}  // namespace ArduinoJson
