// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

namespace ArduinoJson {

class JsonArray;
class JsonObject;
class JsonVariant;

namespace Internals {

class JsonArraySubscript;
template <typename TKey>
class JsonObjectSubscript;

template <typename Writer>
class JsonSerializer {
 public:
  static void serialize(const JsonArray &, Writer &);
  static void serialize(JsonArraySubscript, Writer &);
  static void serialize(const JsonObject &, Writer &);
  template <typename TKey>
  static void serialize(JsonObjectSubscript<TKey>, Writer &);
  static void serialize(JsonVariant, Writer &);
};
}  // namespace Internals
}  // namespace ArduinoJson
