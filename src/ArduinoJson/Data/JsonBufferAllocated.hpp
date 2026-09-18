// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include <cstddef>

namespace ArduinoJson {

class JsonBuffer;

namespace Internals {

class JsonBufferAllocated {
 public:
  static void* operator new(size_t, JsonBuffer*) noexcept;

  // note that operator new above generally works as new w/ std::nothrow
  // no special delete, data just thrown in here and never leaves
};

}  // namespace Internals
}  // namespace ArduinoJson
