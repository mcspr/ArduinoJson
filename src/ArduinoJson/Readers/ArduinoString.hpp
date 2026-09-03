// MIT License

#pragma once

#include "../Configuration.hpp"

#if ARDUINOJSON_ENABLE_ARDUINO_STRING

#include "Base.hpp"
#include "StdString.hpp"

#include <WString.h>

namespace ArduinoJson {
namespace Internals {
namespace Readers {

template <>
struct ReaderImplBase<String> {
  using type = StdString::Reader<String>;
};

template <>
struct ReaderImplBase<StringSumHelper> {
  using type = StdString::Reader<StringSumHelper>;
};

}
}
}

#endif
