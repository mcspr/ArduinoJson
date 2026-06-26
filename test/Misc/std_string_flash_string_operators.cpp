// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#define ARDUINOJSON_CHAR_POINTER_IS_PROGMEM 1
#define ARDUINOJSON_ENABLE_PROGMEM 1

#include <ArduinoJson.h>
#include <catch.hpp>

#define ARDUINOJSON_STD_STRING_TEST_CASE "std::string w/ Strings::FlashString instead of Strings::CharPointer"
#include "std_string.ipp"
