// MIT License

#pragma once

#include <cstddef>
#include <cstdlib>

namespace ArduinoJson {

class Allocator {
 public:
  virtual void* allocate(size_t size) = 0;
  virtual void deallocate(void* ptr) = 0;

 protected:
  ~Allocator() = default;
};

class DefaultAllocator : public Allocator {
 public:
  void* allocate(size_t size) override {
    return malloc(size);
  }

  void deallocate(void* pointer) override {
    free(pointer);
  }

  static Allocator* instance() {
    static DefaultAllocator allocator;
    return &allocator;
  }

 private:
  DefaultAllocator() = default;
  ~DefaultAllocator() = default;
};
}
