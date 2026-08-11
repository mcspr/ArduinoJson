// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "Constant.hpp"

namespace ArduinoJson {
namespace Internals {
namespace TypeTraits {

// nb. polyfill unused due to missing private inheritance check & mandatory complete type assertion
template <typename TBase, typename TDerived>
struct IsBaseOfProbe :
  IntegralConstant<bool, __is_base_of(TBase, TDerived)>::type {

};

}

// A meta-function that returns true if TDerived inherits from TBase
template <typename TBase, typename TDerived>
struct IsBaseOf : TypeTraits::IsBaseOfProbe<TBase, TDerived>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
