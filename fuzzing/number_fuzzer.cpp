#include <ArduinoJson/Numbers/parseNumber.hpp>

using ArduinoJson::Internals::parseJsonNumber;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  // Don't make a copy to ensure the input is NOT null-terminated
  const char* str = reinterpret_cast<const char*>(data);

  parseJsonNumber(str, size);

  return 0;
}
