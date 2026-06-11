// MIT License

#pragma once

#include "../Configuration.hpp"

#if ARDUINOJSON_CHAR_POINTER_IS_PROGMEM
#include "FlashString.hpp"
#endif

#include "CharPointer.hpp"

namespace ArduinoJson {
namespace Internals {
namespace Strings {

#if ARDUINOJSON_CHAR_POINTER_IS_PROGMEM
using FlashString::Length;
using FlashString::Copy;
using FlashString::Equals;
using FlashString::Duplicate;
#else
using CharPointer::Length;
using CharPointer::Copy;
using CharPointer::IsNull;
using CharPointer::Equals;
using CharPointer::Duplicate;
#endif

}
}
}
