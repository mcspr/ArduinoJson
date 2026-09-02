// MIT License

#pragma once

namespace ArduinoJson {
namespace Internals {

template <typename T>
struct AddConst {
  using type = T const;
};

}
}
