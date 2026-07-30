// MIT License

#pragma once

namespace ArduinoJson {
namespace Internals {

struct JsonStringPointer {
  const char* data;
  bool parsed;
};

}  // namespace Internals
}  // namespace ArduinoJson
