#pragma once

#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <tuple>

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

template <>
struct Accessor<const char*> : public std::false_type {};

template <>
struct Accessor<char*> : public Accessor<const char*> {};

template <typename T>
struct Accessor<const T> : public Accessor<T> {};

template <typename T> struct Accessor<T &> : public Accessor<T> {};

template <typename T>
struct Accessor<const T&> : public Accessor<T> {};

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
Value elementToValue(const T& t, PathRef path,
                     std::enable_if_t<hasAccessor<T>, void **> = 0) {
  return Accessor<T>::get(t, path);
}

namespace impl {
   
template <typename KeyT, typename ValueT>
struct AssociativeContainerAccessor : public std::true_type {
  template <typename T>
  static Value get(const T& map, PathRef path)
  {
    auto key = popKey(path);
    if (!key)
       return ValueTag::Object;
    if (key.isIndex())
       return ValueTag::SubValue;

    auto itr = map.find(lex_cast<KeyT>(key.name(), "Not a valid key value!"));
    if (itr != map.end())
       return elementToValue(itr->second, path);

    return ValueTag::Null;
  }
};

struct PointerAccessor : public std::true_type {
  template <typename T>
  static Value get(const T& ptr, PathRef path)
  {
     if (!ptr)
        return ValueTag::Null;
     return elementToValue(*ptr, path);
  }
};

struct IndexContainerAccessor : public std::true_type {
  template <typename T>
  static Value get(const T& vec, PathRef path) {
    auto key = popKey(path);
    if (!key)
       return RangeDefinition{vec.size()};
    if (key.isName())
       return ValueTag::SubValue;

    auto idx = key.index();
    if (idx < vec.size())
       return elementToValue(vec[idx], path);

    return ValueTag::OutOfRange;
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
struct Accessor<std::vector<ValueT>> : public impl::IndexContainerAccessor {
};

template <typename ValueT, size_t Size>
struct Accessor<std::array<ValueT, Size>> : public impl::IndexContainerAccessor {
};

template <typename ValueT>
struct Accessor<std::reference_wrapper<ValueT>> : public std::true_type {
  template <typename T>
  static inline Value get(const T& ref, PathRef path) {
    return elementToValue(ref.get(), path);
  }
};

template <typename ValueT>
struct Accessor<std::shared_ptr<ValueT>> : public impl::PointerAccessor {
};

template <typename ValueT>
struct Accessor<boost::optional<ValueT>> : public impl::PointerAccessor {
};

template <typename ValueT>
struct Accessor<std::unique_ptr<ValueT>> : public impl::PointerAccessor {
};

template <typename ValueT>
struct Accessor<ValueT*> : public impl::PointerAccessor {
};

template <typename ValueT>
struct Accessor<std::weak_ptr<ValueT>> : public std::true_type {
  template <typename T>
  static inline Value get(const T& ptr, PathRef path) {    
    return elementToValue(ptr.lock(), path);
  }
};

template <typename T1, typename T2>
struct Accessor<std::pair<T1, T2>> : public std::true_type {
  template <typename T>
  static inline Value get(const T& ref, PathRef path) {
    auto key = popKey(path);
    if (!key)
       return ValueTag::Object;
    
    if (key.isName())
    {
      if (key == "first")
         return elementToValue(ref.first, path);
      if (key == "second")
         return elementToValue(ref.second, path);
    }
    else
    {
       switch(key.index())
       {
          case 0:
             return elementToValue(std::get<0>(ref), path);
          case 1:
             return elementToValue(std::get<1>(ref), path);
          default:
             return ValueTag::OutOfRange;
       }
    }
    
    return ValueTag::Null;
  }
};

template <typename... Args>
struct Accessor<std::tuple<Args...>> : public std::true_type {
  template<typename T, size_t Idx>
  static Value getByIndex(const T& ref, PathRef path)
  {
    return elementToValue(std::get<Idx>(ref), path);
  }
  
  template<typename T, std::size_t... Idx>
  static constexpr auto idxAccessorsImpl(std::index_sequence<Idx...>)
  {
     using FuncSig = Value (*)(const T& ref, PathRef path);
     return std::array<FuncSig, sizeof...(Idx)>{{ &getByIndex<T, Idx>... }};
  }
  
  template<typename T, std::size_t N, typename Indices = std::make_index_sequence<N>>
  static constexpr auto idxAccessors()
  {
     return idxAccessorsImpl<T>(Indices());
  }
   
  template <typename T>
  static inline Value get(const T& ref, PathRef path) {
    auto key = popKey(path);
    if (!key)
       return RangeDefinition{sizeof...(Args)};
    if (key.isName())
       return ValueTag::SubValue;
    
    static constexpr auto idxAccess = idxAccessors<T, sizeof...(Args)>();
    if (key.index() < sizeof...(Args))
       return idxAccess[key.index()](ref, path);
    
    return ValueTag::OutOfRange;
  }
};

template <typename... Args>
struct Accessor<boost::variant<Args...>> : public std::true_type {
  struct Visitor : public boost::static_visitor<Value>
  {
     PathRef path;
     
     inline Visitor(PathRef p)
      : path(p)
     {}
     
     template<typename T>
     inline Value operator()(const T& val) const
     {
        return elementToValue(val, path);
     }
  };
   
  template <typename T>
  static inline Value get(const T& ref, PathRef path) {     
    return boost::apply_visitor(Visitor{path}, ref);
  }
};
}
