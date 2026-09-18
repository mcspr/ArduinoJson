// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "JsonPrintable.hpp"
#include "JsonWriter.hpp"

template <typename TImpl>
template <typename Print,
  typename ArduinoJson::Internals::EnableIf<
    ArduinoJson::Internals::HasPrint<Print>::value>::type*>
inline size_t ArduinoJson::Internals::JsonPrintable<TImpl>::printTo(Print& print) const {
  JsonWriterSink<Print> sink(print);

  JsonWriter writer(&sink);
  serialize(impl(), writer);

  return writer.bytesWritten();
}
