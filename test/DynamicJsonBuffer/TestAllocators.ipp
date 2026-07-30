// MIT License

#pragma once

#include <ArduinoJson.hpp>
#include <cstdlib>

static inline ArduinoJson::Allocator* defaultAllocator() {
  return ArduinoJson::DefaultAllocator::instance();
}

static constexpr auto EmptyBlockSize =
  size_t{ ArduinoJson::DynamicJsonBuffer::EmptyBlockSize };

class SpyingAllocator final : public ArduinoJson::Allocator {
 public:
  explicit SpyingAllocator(std::ostream& logger) :
    _logger(logger)
  {}

  ~SpyingAllocator() = default;

  void* allocate(size_t n) override {
    _logger << "A" << (n - EmptyBlockSize);
    return defaultAllocator()->allocate(n);
  }

  void deallocate(void* p) override {
    _logger << "F";
    return defaultAllocator()->deallocate(p);
  }

 private:
  std::ostream& _logger;
};

class OnceAllocator final : public ArduinoJson::Allocator {
 public:
  OnceAllocator() noexcept = default;
  explicit OnceAllocator(bool done) noexcept :
    _done(done)
  {}

  ~OnceAllocator() = default;

  void* allocate(size_t bytes) override {
    void* out = nullptr;

    if (!_done) {
      out = defaultAllocator()->allocate(bytes);
      _done = true;
    }

    return out;
  }

  void deallocate(void* ptr) override {
    defaultAllocator()->deallocate(ptr);
  }

 private:
  bool _done{};
};

class CappedAllocator final : public ArduinoJson::Allocator {
 public:
  CappedAllocator() noexcept = default;
  explicit CappedAllocator(size_t capacity) noexcept :
    _capacity(capacity)
  {}

  ~CappedAllocator() = default;

  // similar to OnceAllocator, but take into an account that block is allocated inline
  void* allocate(size_t bytes) override {
    void* out = nullptr;
    const size_t without_block =
      bytes >= EmptyBlockSize
        ? bytes - EmptyBlockSize
        : bytes;

    if ((without_block + _size) < _capacity) {
      out = parentInstance()->allocate(bytes);
      _size += without_block;
    }

    return out;
  }

  void deallocate(void* ptr) override {
    parentInstance()->deallocate(ptr);
  }

 private:
  static Allocator* parentInstance() {
    return ArduinoJson::DefaultAllocator::instance();
  }

  size_t _size{};
  size_t _capacity;
};
