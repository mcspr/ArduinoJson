// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2023
// MIT License

#define ARDUINOJSON_CHAR_POINTER_IS_PROGMEM 1
#define ARDUINOJSON_ENABLE_PROGMEM 1

#include <ArduinoJson.h>
#include <catch.hpp>

#define ARDUINOJSON_UNSIGNED_CHAR_TEST_CASE "signed / unsigned char string w/ Strings::FlashString instead of Strings::CharPointer"
#include "unsigned_char.ipp"
