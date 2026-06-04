// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Constant.hpp"
#include "RemoveReference.hpp"

namespace ArduinoJson {
namespace Internals {
namespace IsBaseOfImpl {

template <typename TBase, typename TDerived>
struct Probe {
  static TrueType probe(const TBase *);
  static FalseType probe(...);
};

template <typename TBase, typename TDerived>
using ProbeImpl = decltype(Probe<TBase, TDerived>::probe(
    static_cast<typename RemoveReference<TDerived>::type *>(nullptr)));

}

// A meta-function that returns true if TDerived inherits from TBase
template <typename TBase, typename TDerived>
struct IsBaseOf : IsBaseOfImpl::ProbeImpl<TBase, TDerived>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
