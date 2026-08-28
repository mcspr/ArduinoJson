#include <ArduinoJson.h>

#include <istream>
#include <streambuf>

class memstream : public std::istream {
  struct membuf : std::streambuf {
    membuf(const uint8_t *p, size_t l) {
      auto *cchar_p = reinterpret_cast<const char *>(p);
      auto *char_p = const_cast<char *>(cchar_p);
      setg(char_p, char_p, char_p + l);
    }
  };

  membuf _buffer;

 public:
  memstream(const uint8_t *p, size_t l)
      : std::istream(&_buffer), _buffer(p, l) {
    rdbuf(&_buffer);
  }
};

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  DynamicJsonBuffer jsonBuffer;
  memstream data_stream(data, size);
  JsonVariant variant = jsonBuffer.parse(data_stream);
  if (variant.success()) {
    std::string json;
    variant.printTo(json);
  }
  return 0;
}
