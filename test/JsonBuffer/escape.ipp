#include <ArduinoJson.h>
#include <string>

// ref. https://github.com/catchorg/Catch2/issues/2967
// help out -i (invisibles) switch, as it does not really work in this case
//
// generate readable escape sequence data like
// > escape(testCase) := "\x7f"
// instead of
// > testCase := "''"

namespace {

std::string escape(const std::string& s) {
  using ArduinoJson::Internals::Character::Serialization::Nibbles;

  std::string out;
  char tmp[] = "\\x00";

  for (char c : s) {
    switch (c) {
    case '\a':
      out.append("\\a");
      break;

    case '\b':
      out.append("\\b");
      break;

    case '\t':
      out.append("\\t");
      break;

    case '\n':
      out.append("\\n");
      break;

    case '\v':
      out.append("\\v");
      break;

    case '\f':
      out.append("\\f");
      break;

    case '\r':
      out.append("\\r");
      break;

    case '\\':
      out.append("\\\\");
      break;

    default:
      if (!std::isprint(c)) {  // invisibles as hex escape
        const auto nibbles = Nibbles(c);
        tmp[2] = nibbles.left();
        tmp[3] = nibbles.right();
        out.append(tmp);
      } else {  // printable ASCII range
        out.push_back(c);
      }
      break;
    }
  }

  return out;
}

}
