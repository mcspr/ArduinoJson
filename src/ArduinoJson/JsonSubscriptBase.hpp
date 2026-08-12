// MIT License

#pragma once

#include "JsonImplicitConversions.hpp"

#include "JsonVariantComparisons.hpp"
#include "JsonVariantOr.hpp"

#include "Serialization/JsonPrintable.hpp"

#include "TypeTraits/IsSubscript.hpp"

namespace ArduinoJson {
namespace Internals {

// respect subscript member constness, not the object itself
// for non-const, explicitly sets operator= and operator T() conversions
// for const, deletes them (xxx: note that base class *may* keep copy operators, take care to delete those)

template <typename T, typename TImpl>
struct JsonSubscriptConversions {
  using type = JsonImplicitConversions<TImpl, JsonImplicitAnyReference>;
};

template <typename T, typename TImpl>
struct JsonSubscriptConversions<T const, TImpl> {
  using type = JsonImplicitConversions<TImpl, JsonImplicitConstReference>;
};

// subscript types inherit from this instead of variant base,
// but are expected to keep other JsonVariant properties
template <typename T, typename TImpl>
class JsonSubscriptBase :
    public JsonPrintable<TImpl>,
    public JsonSubscriptConversions<T, TImpl>::type,
    public JsonVariantComparisons<TImpl>,
    public JsonVariantOr<TImpl>,
    public JsonSubscriptTag,
    public JsonVariantTag {

};

}  // namespace Internals
}  // namespace ArduinoJson
