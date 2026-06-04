// MIT License

#pragma once

#include "Constant.hpp"

namespace ArduinoJson {
namespace Internals {
namespace NotImpl {

// Replace TrueType with FalseType
// Replace FalseType with TrueType
template <typename Boolean>
struct Not : BooleanConstant<!bool(Boolean::value)> {
};

}

// aka -std=c++17 std::negation
using NotImpl::Not;

}
}
