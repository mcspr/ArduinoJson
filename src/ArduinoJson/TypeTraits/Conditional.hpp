// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#pragma once

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

template <bool Condition>
struct ConditionalImpl {
  template <typename True, typename>
  using type = True;
};

template <>
struct ConditionalImpl<false> {
  template <typename, typename False>
  using type = False;
};

}

template <bool Condition, class True, class False>
using Conditional =
    typename TypeTraits::ConditionalImpl<Condition>::template type<True, False>;

}
}
