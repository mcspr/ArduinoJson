// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../Data/JsonFloat.hpp"
#include "../Data/JsonInteger.hpp"
#include "../Data/JsonNull.hpp"
#include "../Data/JsonStringPointer.hpp"

namespace ArduinoJson {

class JsonArray;
class JsonObject;
class JsonVariant;

namespace Internals {

struct JsonVariantUndefined;

class JsonMutableArraySubscript;
class JsonConstArraySubscript;

template <typename TKey>
class JsonMutableObjectSubscript;

template <typename TKey>
class JsonConstObjectSubscript;

template <typename Writer>
class JsonSerializer {
 public:
  static void serialize(const JsonArray &, Writer &);
  static void serialize(JsonMutableArraySubscript, Writer &);
  static void serialize(JsonConstArraySubscript, Writer &);
  static void serialize(const JsonObject &, Writer &);
  template <typename TKey>
  static void serialize(const JsonConstObjectSubscript<TKey>&, Writer &);
  template <typename TKey>
  static void serialize(const JsonMutableObjectSubscript<TKey>&, Writer &);
  static void serialize(JsonVariant, Writer &);
 private:
  struct VariantVisitor {
   private:
    Writer& _writer;

   public:
    explicit VariantVisitor(Writer& writer) :
      _writer(writer)
    {}

    void Operator(Internals::JsonVariantUndefined);
    void Operator(JsonNull);
    void Operator(bool);
    void Operator(const JsonObject*);
    void Operator(JsonObject*);
    void Operator(const JsonArray*);
    void Operator(JsonArray*);
    void Operator(JsonFloat);
    void Operator(JsonInteger);
    void Operator(JsonUnsignedInteger);
    void Operator(JsonStringPointer);
  };
};
}  // namespace Internals
}  // namespace ArduinoJson
