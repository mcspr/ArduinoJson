// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

namespace ArduinoJson {
namespace Internals {

template <bool Condition>
struct Conditional {
  template <typename True, typename>
  using type = True;
};

template <>
struct Conditional<false> {
  template <typename, typename False>
  using type = False;
};

template <bool Condition, class True, class False>
using ConditionalType =
    typename Conditional<Condition>::template type<True, False>;

}
}
