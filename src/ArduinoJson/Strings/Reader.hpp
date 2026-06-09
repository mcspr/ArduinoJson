// MIT License

#pragma once

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

template <typename TImpl>
struct ReaderBase {
 private:
  const char* _ptr;
  const char* _end;

 public:
  ReaderBase(const void* ptr, size_t size) :
    _ptr(ptr ? reinterpret_cast<const char*>(ptr) : ""),
    _end(ptr ? (_ptr + size) : "")
  {}

  void move() {
    if (_ptr < _end)
      ++_ptr;
  }

  char current() const {
    if (_ptr < _end)
      return TImpl::read(_ptr);

    return '\0';
  }

  char next() const {
    if ((_ptr + 1) < _end)
      return TImpl::read(_ptr + 1);

    return '\0';
  }
};

}
}
