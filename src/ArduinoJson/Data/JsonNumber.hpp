// MIT License

#pragma once

#include "../Numbers/convertNumber.hpp"
#include "../TypeTraits/IsSame.hpp"

#include "JsonInteger.hpp"
#include "JsonFloat.hpp"

#include <cstdint>

namespace ArduinoJson {
namespace Internals {

template <typename TSigned, typename TUnsigned, typename TFloat>
union NumberValueImpl;

template <typename TSigned, typename TUnsigned>
union NumberValueImpl<TSigned, TUnsigned, float> {
  enum class Type : uint8_t {
    Invalid = 0,
    Float = 1,
    SignedInteger = 3,
    UnsignedInteger = 4,
  };

  struct Invalid {
    Type type;
  };

  struct Float {
    Type type;
    float value;
  };

  struct SignedInteger {
    Type type;
    TSigned value;
  };

  struct UnsignedInteger {
    Type type;
    TUnsigned value;
  };

  NumberValueImpl() :
    invalid({Type::Invalid})
  {}

  NumberValueImpl(TSigned value) :
    asSignedInteger({Type::SignedInteger, value})
  {}

  NumberValueImpl(TUnsigned value) :
    asUnsignedInteger({Type::UnsignedInteger, value})
  {}

  NumberValueImpl(float value) :
    asFloat({Type::Float, value})
  {}

  template <typename T>
  ConvertResult<T> convertTo() const;

  Invalid invalid;
  Float asFloat;
  SignedInteger asSignedInteger;
  UnsignedInteger asUnsignedInteger;
};

template <typename TSigned, typename TUnsigned>
union NumberValueImpl<TSigned, TUnsigned, double> {
  enum class Type : uint8_t {
    Invalid = 0,
    Float = 1,
    Double = 2,
    SignedInteger = 3,
    UnsignedInteger = 4,
  };

  struct Invalid {
    Type type;
  };

  struct SignedInteger {
    Type type;
    TSigned value;
  };

  struct UnsignedInteger {
    Type type;
    TUnsigned value;
  };

  struct Float {
    Type type;
    float value;
  };

  struct Double {
    Type type;
    double value;
  };

  NumberValueImpl() :
    invalid({Type::Invalid})
  {}

  NumberValueImpl(TSigned value) :
    asSignedInteger({Type::SignedInteger, value})
  {}

  NumberValueImpl(TUnsigned value) :
    asUnsignedInteger({Type::UnsignedInteger, value})
  {}

  NumberValueImpl(float value) :
    asFloat({Type::Float, value})
  {}

  NumberValueImpl(double value) :
    asDouble({Type::Double, value})
  {}

  template <typename T>
  ConvertResult<T> convertTo() const;

  Invalid invalid;
  SignedInteger asSignedInteger;
  UnsignedInteger asUnsignedInteger;
  Float asFloat;
  Double asDouble;
};

template <typename TSigned, typename TUnsigned, typename TFloat>
class NumberImpl {
 public:
  using ValueType =
    NumberValueImpl<TSigned, TUnsigned, TFloat>;
  using Type =
    typename ValueType::Type;

  NumberImpl() :
    _value()
  {}

  NumberImpl(TSigned value) :
    _value(value)
  {}

  NumberImpl(TUnsigned value) :
    _value(value)
  {}

  NumberImpl(float value) :
    _value(value)
  {}

  template <typename T,
    typename Internals::EnableIf<
      And<IsSame<T, TFloat>,
          IsSame<T, double>>::value>::type* = nullptr>
  NumberImpl(T value) :
    _value(value)
  {}

  template <typename T>
  ConvertResult<T> convertTo() const {
    return _value.template convertTo<T>();
  }

  Type type() const {
    return _value.invalid.type;
  }

  bool ok() const {
    return _value.invalid.type != Type::Invalid;
  }

  explicit operator bool() const {
    return ok();
  }

 private:
  ValueType _value;
};

template <typename TSigned, typename TUnsigned>
template <typename T>
inline ConvertResult<T>
NumberValueImpl<TSigned, TUnsigned, float>::convertTo() const {
  if (asFloat.type == Type::Float)
    return convertNumber<T>(asFloat.value);

  else if (asSignedInteger.type == Type::SignedInteger)
    return convertNumber<T>(asSignedInteger.value);

  else if (asUnsignedInteger.type == Type::UnsignedInteger)
    return convertNumber<T>(asUnsignedInteger.value);

  return ConvertResult<T>{};
}

template <typename TSigned, typename TUnsigned>
template <typename T>
inline ConvertResult<T>
NumberValueImpl<TSigned, TUnsigned, double>::convertTo() const {
  if (asFloat.type == Type::Float)
    return convertNumber<T>(asFloat.value);

  else if (asDouble.type == Type::Double)
    return convertNumber<T>(asDouble.value);

  else if (asSignedInteger.type == Type::SignedInteger)
    return convertNumber<T>(asSignedInteger.value);

  else if (asUnsignedInteger.type == Type::UnsignedInteger)
    return convertNumber<T>(asUnsignedInteger.value);

  return ConvertResult<T>{};
}

}  // namespace Internals

using JsonNumber = Internals::NumberImpl<
  Internals::JsonInteger,
  Internals::JsonUnsignedInteger,
  Internals::JsonFloat>;

}  // namespace ArduinoJson
