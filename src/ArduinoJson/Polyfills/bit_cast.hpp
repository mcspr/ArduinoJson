// MIT License

#pragma once

#include <version>

#if defined(__cpp_lib_bit_cast)
#include <bit>
#else
#include "../TypeTraits/IsTrivial.hpp"
#include "../TypeTraits/And.hpp"

#include <cstring>
#endif

namespace ArduinoJson {
namespace Internals {

#if defined(__cpp_lib_bit_cast)
using ::std::bit_cast;
#else
// Older compilers (e.g. gcc4.x) will not always correctly resolve true alignment of the pointer / reference origin,
// hence param type is `From` instead of `const From&`; note that current use-case is limited to implicitly aligned data,
// meaning the copy operation itself should be omitted and optimized into a single load w/o invoking memcpy
template <typename To, typename From>
inline To bit_cast(From raw_data) {
  static_assert(sizeof(To) == sizeof(From), "");
  static_assert(And<IsTrivial<To>, IsTrivial<From>>::value, "");

  To out;
  std::memcpy(&out, &raw_data, sizeof(To));
  return out;
}
#endif

}  // namespace Internals
}  // namespace ArduinoJson
