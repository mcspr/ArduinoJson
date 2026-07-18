// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include <cstddef>
#include <cstdint>

namespace ArduinoJson {
namespace Internals {

// Decorator on top of Print to allow indented output.
// This class is used by JsonPrintable::prettyPrintTo() but can also be used
// for your own purpose, like logging.

struct IndentedPrintOptions {
  uint8_t level : 4;
  uint8_t tabSize : 3;
  bool isNewLine : 1;
};

static constexpr uint8_t MaxIndentedPrintLevel = 15;  // 0b1111
static constexpr uint8_t MaxIndentedPrintTabSize = 7; // 0b111

static_assert(sizeof(IndentedPrintOptions) == 1, "");
static constexpr auto DefaultIndentedOptions =
  IndentedPrintOptions{ 0, 2, true };

template <typename Print>
class IndentedPrint {
 public:
  explicit IndentedPrint(Print &p) :
    _sink(&p)
  {}

  size_t print(char c) {
    size_t n = 0;
    if (_isNewLine())
      n += writeTabs();
    n += _sink->print(c);
    _isNewLine(c == '\n');
    return n;
  }

  size_t print(const char *s) {
    // TODO: optimize
    size_t n = 0;
    while (*s) n += print(*s++);
    return n;
  }

  // Adds one level of indentation
  void indent() {
    const auto level = _level();
    if (level < MaxIndentedPrintLevel)
      _level(static_cast<uint8_t>(level + 1));
  }

  // Removes one level of indentation
  void unindent() {
    const auto level = _level();
    if (level > 0)
      _level(static_cast<uint8_t>(level - 1));
  }

  // Set the number of space printed for each level of indentation
  void setTabSize(uint8_t n) {
    if (n < MaxIndentedPrintTabSize)
      _tabSize(n);
  }

 private:
  uint8_t _level() const {
    return _opts.level;
  }

  void _level(uint8_t value) {
    _opts.level = static_cast<uint8_t>(value & MaxIndentedPrintLevel);
  }

  uint8_t _tabSize() const {
    return _opts.tabSize;
  }

  void _tabSize(uint8_t value) {
    _opts.tabSize = static_cast<uint8_t>(value & MaxIndentedPrintTabSize);
  }

  bool _isNewLine() const {
    return _opts.isNewLine;
  }

  void _isNewLine(bool value) {
    _opts.isNewLine = value;
  }

  size_t writeTabs() {
    const auto level = _level();
    const auto tabSize = _tabSize();

    size_t n = 0;
    for (int i = 0; i < level * tabSize; i++)
      n += _sink->print(' ');

    return n;
  }

  Print* _sink;
  IndentedPrintOptions _opts = DefaultIndentedOptions;
};
}  // namespace Internals
}  // namespace ArduinoJson
