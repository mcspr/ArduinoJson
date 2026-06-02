// MIT License

#pragma once

#include "../TypeTraits/RemoveConstReference.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename TString, typename Enable = void>
struct StringTraitsImpl {
  static const bool has_append = false;
  static const bool has_equals = false;
};

template <typename TString,
  typename Enable = void,
  typename TNoRef = typename RemoveReference<TString>::type,
  typename TBase = StringTraitsImpl<TNoRef, Enable>>
struct StringTraits : public TBase {
};

}  // namespace Internals
}  // namespace ArduinoJson

