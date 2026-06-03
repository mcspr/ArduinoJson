// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#ifdef __cplusplus

#include "ArduinoJson.hpp"

using ArduinoJson::JsonArray;
using ArduinoJson::JsonNull;
using ArduinoJson::JsonObject;
using ArduinoJson::JsonVariant;
using ArduinoJson::JsonSpan;

using ArduinoJson::DynamicJsonBuffer;
using ArduinoJson::StaticJsonBuffer;

using ArduinoJson::RawJson;

#else

#error ArduinoJson requires a C++ compiler, please change file extension to .cc or .cpp

#endif
