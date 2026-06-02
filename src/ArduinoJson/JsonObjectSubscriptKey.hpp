// MIT License

#pragma once

#include <cstddef>

namespace ArduinoJson {
namespace Internals {

// Subscript key string object proxy / holder
// Generally, a shortcut for obj.set(..., value)
// .get() always returns an ephemeral reference

// Only expected to be used for user-provided data
// String dispatch and storage happens elsewhere

// generic containers like std::string

template <typename TString>
class JsonObjectSubscriptKey {
 public:
  JsonObjectSubscriptKey() = default;
  explicit JsonObjectSubscriptKey(TString str) :
    _str(str)
  {}

  const TString& get() const {
    return _str;
  }

 private:
  TString _str;
};

template <typename TString>
class JsonObjectSubscriptKey<TString&&> :
  public JsonObjectSubscriptKey<TString> {
};

template <typename TString>
class JsonObjectSubscriptKey<const TString&> {
 public:
  JsonObjectSubscriptKey() = delete;
  explicit JsonObjectSubscriptKey(const TString& ref) :
    _ref(ref)
  {}

  const TString& get() const {
    return *_ref;
  }

 private:
  const TString* _ref;
};

// literals, stack or globals

template <typename TChar, size_t Size>
class JsonObjectSubscriptKey<TChar[Size]> {
 public:
  JsonObjectSubscriptKey() = default;
  explicit JsonObjectSubscriptKey(TChar (&str)[Size]) :
    _str(&str[0])
  {}

  TChar *get() const {
    return _str;
  }

 private:
  TChar *_str{};
};

template <typename TChar, size_t Size>
class JsonObjectSubscriptKey<TChar(&)[Size]> :
  public JsonObjectSubscriptKey<TChar[Size]> {
};

template <typename TChar>
class JsonObjectSubscriptKey<TChar*> {
 public:
  JsonObjectSubscriptKey() = default;
  explicit JsonObjectSubscriptKey(TChar *str) :
    _str(str)
  {}

  TChar *get() const {
    return _str;
  }

 private:
  TChar *_str{};
};

// stack or globals, passed by value

template <typename TChar>
class JsonObjectSubscriptKey<TChar*&> {
 public:
  JsonObjectSubscriptKey() = default;
  explicit JsonObjectSubscriptKey(TChar *&str) :
    _str(str)
  {}

  TChar *get() const {
    return _str;
  }

 private:
  TChar *_str{};
};

template <typename TChar>
class JsonObjectSubscriptKey<const TChar*&> {
 public:
  JsonObjectSubscriptKey() = default;
  explicit JsonObjectSubscriptKey(const TChar *&str) :
    _str(str)
  {}

  const TChar *get() const {
    return _str;
  }

 private:
  const TChar *_str{};
};

// TODO clever templating?

}  // namespace Internals
}  // namespace ArduinoJson
