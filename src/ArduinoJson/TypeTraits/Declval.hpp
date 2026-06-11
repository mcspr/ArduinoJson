// MIT License

#include "RemoveConstReference.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename T>
typename RemoveConstReference<T>::type Declval();

}
}
