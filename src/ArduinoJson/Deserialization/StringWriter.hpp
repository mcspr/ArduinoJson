// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../TypeTraits/IsConst.hpp"

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

template <typename TChar>
class StringWriter {
 static_assert(!Internals::IsConst<TChar>::value, "");

 public:
  class String {
   public:
    String(StringWriter &parent) :
      _parent(parent),
      _startPtr(parent._writePtr)
    {}

    void append(char c) {
      _parent.append(c);
    }

    const char* c_str() const {
      if (_parent.append(0))
        return reinterpret_cast<const char*>(_startPtr);

      return nullptr;
    }

   private:
    StringWriter<TChar> &_parent;
    TChar *_startPtr;
  };

  StringWriter(TChar *buffer, size_t len) :
    _startPtr(buffer),
    _writePtr(buffer),
    _len(len)
  {}

  String startString() {
    return String(*this);
  }

 private:
  void move() {
    if (writable())
      ++_writePtr;
  }

  bool append(char c) {
    bool out = false;

    if (writable()) {
      *_writePtr = TChar(c);
      out = true;
    }

    move();

    return out;
  }

  bool writable() const {
    return _writePtr < _startPtr + _len;
  }

  TChar *_startPtr;
  TChar *_writePtr;

  size_t _len;
};
}  // namespace Internals
}  // namespace ArduinoJson
