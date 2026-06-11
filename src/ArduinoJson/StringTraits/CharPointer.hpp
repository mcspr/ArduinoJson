// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#pragma once

#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsChar.hpp"
#include "../TypeTraits/IsConst.hpp"
#include "../TypeTraits/Not.hpp"

#include "../Readers/CharPointer.hpp"
#include "../Strings/CharPointer.hpp"

#include "StringTraitsBase.hpp"

#include <cstddef>
#include <cstring>

namespace ArduinoJson {
namespace Internals {

struct CharPointerTraitsBase : StringTraitsTag {
  typedef Readers::CharPointer::Reader Reader;
  typedef Strings::CharPointer::Equals Equals;
  typedef Strings::CharPointer::IsNull IsNull;
  typedef Strings::CharPointer::Reference Reference;
};

template <typename TChar, typename = void>
struct CharPointerTraits : CharPointerTraitsBase {
};

template <typename TChar>
struct CharPointerTraits<TChar,
  typename EnableIf<Not<IsConst<TChar>>::value>::type> : CharPointerTraitsBase {

  typedef Strings::CharPointer::Duplicate Duplicate;
};

// char*, unsigned char*, signed char*
// const char*, const unsigned char*, const signed char*
template <typename TChar>
struct StringTraitsImpl<TChar*, typename EnableIf<IsChar<TChar>::value>::type>
    : public CharPointerTraits<TChar, void> {
};

// char[], unsigned char[], signed char[]
// const char[], const unsigned char[], const signed char[]
template <typename TChar, size_t Size>
struct StringTraitsImpl<TChar[Size], typename EnableIf<IsChar<TChar>::value>::type>
    : public StringTraitsImpl<TChar*> {
};

}  // namespace Internals
}  // namespace ArduinoJson
