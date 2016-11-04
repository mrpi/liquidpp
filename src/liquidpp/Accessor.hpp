#pragma once

#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/variant/get.hpp>

#include "Key.hpp"
#include "config.h"

#include "Value.hpp"
#include "Misc.hpp"

namespace liquidpp {

using ValueGetter = std::function<Value(PathRef)>;

template <typename T, typename = void> struct Accessor : public std::false_type {};

template <typename T>
struct Accessor<const T> : public Accessor<T> {};

template <typename T> struct Accessor<T &> : public Accessor<T> {};

template <typename T>
constexpr bool hasAccessor = Accessor<T>::value;

inline Value toValue(Value v) { return v; }

inline Value toValue(std::string v) { return Value(std::move(v)); }

inline Value toValue(const char *v) { return Value(std::string{v}); }

template <size_t Len> Value toValue(const char (&v)[Len]) {
  return Value(std::string{v});
}

inline Value toValue(bool b) { return Value::fromBool(b); }

template <typename T>
Value toValue(
    T t, std::enable_if_t<!hasAccessor<T> && std::is_integral<T>::value,
                          void **> = 0) {
  return Value::fromIntegral(t);
}

template <typename T>
Value toValue(
    T t,
    std::enable_if_t<!hasAccessor<T> && std::is_floating_point<T>::value,
                     void **> = 0) {
  return Value::fromFloatingPoint(t);
}

template <typename T>
Value elementToValue(T &&t, PathRef path,
                     std::enable_if_t<!hasAccessor<T>, void **> = 0) {
  if (!path.empty())
    return ValueTag::SubValue;
  return toValue(std::forward<T>(t));
}

template <typename T>
Value elementToValue(T &&t, PathRef path,
                     std::enable_if_t<hasAccessor<T>, void **> = 0) {
  return Accessor<T>::get(std::forward<T>(t))(path);
}

namespace impl {
template <typename KeyT, typename ValueT>
struct AssociativeContainerAccessor : public std::true_type {
  template <typename T> static auto get(T &&map) {
    return [map = std::forward<T>(map)](PathRef path)->Value {
      auto key = popKey(path);
      if (!key)
        return ValueTag::Object;
      if (key.isIndex())
        return ValueTag::SubValue;

      auto itr = map.find(lex_cast<KeyT>(key.name(), "Not a valid key value!"));
      if (itr != map.end())
        return elementToValue(itr->second, path);

      return ValueTag::Null;
    };
  }
};
}

template <typename KeyT, typename ValueT>
struct Accessor<std::map<KeyT, ValueT>>
    : public impl::AssociativeContainerAccessor<KeyT, ValueT> {};

template <typename KeyT, typename ValueT>
struct Accessor<std::unordered_map<KeyT, ValueT>>
    : public impl::AssociativeContainerAccessor<KeyT, ValueT> {};

template <typename ValueT>
struct Accessor<std::vector<ValueT>> : public std::true_type {
  template <typename T> static auto get(T &&vec) {
    return [vec = std::forward<T>(vec)](PathRef path)->Value {
      auto key = popKey(path);
      if (!key)
        return RangeDefinition{vec.size()};
      if (key.isName())
        return ValueTag::SubValue;

      auto idx = key.index();
      if (idx < vec.size())
        return elementToValue(vec[idx], path);

      return ValueTag::OutOfRange;
    };
  }
};
}
