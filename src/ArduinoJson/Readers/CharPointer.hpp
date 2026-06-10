// MIT License

#pragma once

#include "Base.hpp"

#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsChar.hpp"

#include <cstddef>

namespace ArduinoJson {
namespace Internals {
namespace Readers {
namespace CharPointer {

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

struct ReaderImpl {
  static char read(const void* ptr) {
    return *reinterpret_cast<const char *>(ptr);
  }
};

typedef ReaderBase<ReaderImpl> Reader;

}

template <typename TChar>
struct ReaderImplBase<TChar*, typename EnableIf<IsChar<TChar>::value>::type>
  : CharPointer::Reader {
};

}
}
}
