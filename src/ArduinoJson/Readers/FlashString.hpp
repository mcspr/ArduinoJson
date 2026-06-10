// MIT License

#pragma once

#include "../Configuration.hpp"

#if ARDUINOJSON_ENABLE_PROGMEM

#include <Arduino.h>

#include "Base.hpp"
#include "CharPointer.hpp"

class __FlashStringHelper;

namespace ArduinoJson {
namespace Internals {
namespace Readers {
namespace FlashString {

struct ReaderImpl {
  static char read(const void* ptr) {
    return pgm_read_byte_near(ptr);
  }
};

typedef Readers::CharPointer::ReaderBase<ReaderImpl> Reader;

}

template <>
struct ReaderImplBase<const __FlashStringHelper*>
  : FlashString::Reader {
};

}
}
}

#endif
