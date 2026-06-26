// MIT License

#pragma once

#include <memory>

namespace ArduinoJson {

struct JsonParserStopToken;

namespace Internals {

class JsonParserStoppable {
 protected:
  JsonParserStopToken makeStopToken();

 private:
  bool _stop{};
};

}

struct JsonParserStopToken {
  JsonParserStopToken() = delete;

  JsonParserStopToken(const JsonParserStopToken&) noexcept = default;
  JsonParserStopToken& operator=(const JsonParserStopToken&) noexcept = default;

  JsonParserStopToken(JsonParserStopToken&&) noexcept = default;
  JsonParserStopToken& operator=(JsonParserStopToken&&) noexcept = default;

  void request_stop() noexcept {
    *_stop = true;
  }

  void operator()() {
    request_stop();
  }

  bool is_stopped() const {
    return *_stop;
  }

  explicit operator bool() const {
    return is_stopped();
  }

 private:
  friend class Internals::JsonParserStoppable;

  explicit JsonParserStopToken(bool& ref) :
    _stop(std::addressof(ref))
  {}

  explicit JsonParserStopToken(bool* ptr) :
    _stop(ptr)
  {}

  bool* _stop;
};

namespace Internals {

inline JsonParserStopToken JsonParserStoppable::makeStopToken() {
  return JsonParserStopToken(_stop);
}

}
}
