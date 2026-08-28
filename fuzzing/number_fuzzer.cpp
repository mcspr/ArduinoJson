#include <ArduinoJson.h>
#include "ArduinoJson/Numbers/parseNumber.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  // Make a copy to ensure the input is null-terminated
  std::string str(reinterpret_cast<const char*>(data), size);

  ArduinoJson::Internals::JsonNumberParser::parse(str.c_str(), str.length());

  return 0;
}
