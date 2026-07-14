// MIT License

#pragma once

#include "Constant.hpp"
#include "RemoveReference.hpp"

namespace ArduinoJson {
namespace Internals {
namespace TypeTraitsImpl {

template <template <typename...> class TBase, typename TDerived>
struct IsBaseInstantiationOfProbe {
  template <typename... Args>
  static TrueType probe(const TBase<Args...> *);
  static FalseType probe(...);
};

template <template <typename...> class TBase, typename TDerived>
using IsBaseInstantiationOfProbeImpl = decltype(IsBaseInstantiationOfProbe<TBase, TDerived>::probe(
    static_cast<typename RemoveReference<TDerived>::type *>(nullptr)));

}

// A meta-function that returns true if TDerived inherits from TBase
template <template <typename...> class TBase, typename TDerived>
struct IsBaseInstantiationOf : TypeTraitsImpl::IsBaseInstantiationOfProbeImpl<TBase, TDerived>::type {
};

}  // namespace Internals
}  // namespace ArduinoJson
