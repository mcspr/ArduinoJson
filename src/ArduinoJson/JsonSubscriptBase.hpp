// MIT License

#pragma once

#include "JsonImplicitConversions.hpp"
#include "JsonImplicitReferenceConversions.hpp"

#include "JsonVariantComparisons.hpp"
#include "JsonVariantOr.hpp"
#include "JsonVariantSubscripts.hpp"

#include "Serialization/JsonPrintable.hpp"

#include "TypeTraits/IsConst.hpp"
#include "TypeTraits/Conditional.hpp"

namespace ArduinoJson {
namespace Internals {

// respect subscript member constness, not the object itself
// for non-const, explicitly sets operator= and operator T() conversions
// for const, deletes them (xxx: note that base class *may* keep copy operators, take care to delete those)

template <typename T, typename TImpl>
using JsonSubscriptReferenceConversions =
  Conditional<IsConst<T>::value,
    JsonImplicitConstReferenceConversions<TImpl>,
    JsonImplicitReferenceConversions<TImpl>>;

// subscript types inherit from this instead of variant base,
// but are expected to keep other JsonVariantBase properties
template <typename T, typename TImpl>
class JsonSubscriptBase :
    public JsonPrintable<TImpl>,
    public JsonImplicitConversions<TImpl>,
    public JsonSubscriptReferenceConversions<T, TImpl>,
    public JsonVariantComparisons<TImpl>,
    public JsonVariantOr<TImpl>,
    public JsonVariantSubscripts<TImpl>,
    public JsonVariantTag {

};

}  // namespace Internals
}  // namespace ArduinoJson
