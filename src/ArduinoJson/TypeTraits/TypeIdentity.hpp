// MIT License

namespace ArduinoJson {
namespace Internals {

template <typename T>
struct TypeIdentity {
  using type = T;
};

template <typename T>
using TypeIdentityType = typename TypeIdentity<T>::type;

}
}
