// MIT License

#pragma once

#include "JsonBufferBase.hpp"

namespace ArduinoJson {
namespace Internals {

struct EmptyJsonBuffer final : public JsonBufferBase<EmptyJsonBuffer> {
  static EmptyJsonBuffer* instance() {
    static EmptyJsonBuffer jsonBuffer;
    return &jsonBuffer;
  }

  void *alloc(size_t) override {
    return nullptr;
  }

 protected:
  ~EmptyJsonBuffer() {}
};

}
}
