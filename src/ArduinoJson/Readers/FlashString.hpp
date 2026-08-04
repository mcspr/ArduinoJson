// MIT License

#pragma once

#include "../Configuration.hpp"

#if ARDUINOJSON_ENABLE_PROGMEM

#include <Arduino.h>

#include "Base.hpp"
#include "CharPointer.hpp"

#include "../Strings/FlashString.hpp"

class __FlashStringHelper;

namespace ArduinoJson {
namespace Internals {
namespace Readers {
namespace FlashString {

struct Reader : Readers::CharPointer::ReaderBase<Reader> {
  using Copy = Strings::FlashString::Copy;
  using Readers::CharPointer::ReaderBase<Reader>::ReaderBase;
};

}

template <>
struct ReaderImplBase<const __FlashStringHelper*>
  : FlashString::Reader {
};

}
}
}

#endif
