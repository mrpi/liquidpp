#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <utility>

#include <boost/container/flat_map.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>

#include "config.h"

#include "Accessor.hpp"
#include "Key.hpp"

#include "accessors/Accessors.hpp"

#include "external/short_alloc.h"

namespace liquidpp {
class Context {
private:
  const Context *mParent{nullptr};
  Context *mDocumentScopeContext{nullptr};
  using MapValue = boost::variant<Value, ValueGetter>;
  using MapValuePtr = boost::variant<const Value*, const ValueGetter*>;

  using MapType = std::pair<const std::string, MapValue>;
#ifdef _MSC_VER
  using StorageAllocator = std::allocator<MapType>;
#else
  using StorageAllocator =
     hhinnant::short_alloc<MapType, 256>;
  StorageAllocator::arena_type mArena;
#endif

  using StorageT =
      std::map<std::string, MapValue, std::less<void>, StorageAllocator>;

#ifdef _MSC_VER
  StorageT mValues;
#else
  StorageT mValues{ StorageAllocator(mArena) };
#endif
  ValueGetter mAnonymous;
  boost::optional<std::locale> mLocale{std::locale()};

  size_t mMaxOutputSize{8 * 1024 * 1024};
  size_t mMinOutputPer1024Loops{mMaxOutputSize / 4};
  size_t mRecursiveDepth{0};

public:
  Context() = default;

  explicit Context(const Context *parent)
      : mParent(parent), mDocumentScopeContext(this), mLocale(boost::none),
        mMaxOutputSize(parent->mMaxOutputSize),
        mMinOutputPer1024Loops{parent->mMinOutputPer1024Loops} {
    assert(mParent->mDocumentScopeContext == nullptr);
  }

  explicit Context(Context *parent)
      : mParent(parent), mDocumentScopeContext(mParent->mDocumentScopeContext),
        mLocale(boost::none), mMaxOutputSize(parent->mMaxOutputSize),
        mMinOutputPer1024Loops{parent->mMinOutputPer1024Loops},
        mRecursiveDepth{parent->mRecursiveDepth} {
    assert(mDocumentScopeContext != nullptr);
  }

  Context(std::initializer_list<StorageT::value_type> entries)
      : 
#ifdef _MSC_VER
     mValues(entries)
#else
     mValues(entries, StorageAllocator{ mArena })
#endif
  {}

  const std::locale &locale() const {
    if (mLocale)
      return *mLocale;
    return mParent->locale();
  }

  void setLocale(const std::locale &loc) { mLocale = loc; }

  Context &documentScopeContext() {
    assert(mDocumentScopeContext);
    return *mDocumentScopeContext;
  }

  size_t maxOutputSize() const { return mMaxOutputSize; }

  void setMaxOutputSize(size_t val) { mMaxOutputSize = val; }

  size_t minOutputPer1024Loops() const { return mMinOutputPer1024Loops; }

  void setMinOutputPer1024Loops(size_t val) { mMinOutputPer1024Loops = val; }

  size_t &recursiveDepth() { return mRecursiveDepth; }

  Value get(string_view pathStr) const {
    auto p = toPath(pathStr);
    return get(p);
  }

private:
  Value getFromValues(const MapValuePtr& ptr, PathRef path) const {
    if (ptr == MapValuePtr{})
       return Value{};
     
    if (ptr.which() == 1) {
      auto &valueGetter = *boost::get<const ValueGetter*>(ptr);
      auto res = valueGetter(path);
      if (res == ValueTag::SubValue) {
        auto lastKey = popLastKey(path);

        if (lastKey == "size") {
          Value parent = valueGetter(path);
          return toValue(parent.size());
        }

        if (lastKey == "first" || lastKey == "last") {
          Value parent = valueGetter(path);
          if (parent.isRange()) {
            auto &range = parent.range();
            auto size = range.size();
            if (size == 0)
              return ValueTag::Null;

            auto idx = range.index(lastKey == "last" ? size - 1 : 0);
            auto elementPath = path + Key{idx};
            return valueGetter(elementPath);
          }
        }
      }
      return res;
    }

    auto& val = *boost::get<const Value*>(ptr);
    if (!path.empty()) {
      if (path.size() == 1 && path[0] == "size")
        return toValue(val.toString().size());

      return ValueTag::SubValue;
    }

    return val.asReference();
  }
  
  inline MapValuePtr getPtr(PathRef& path) const
  {
    if (path.empty())
      throw std::runtime_error("Can't handle empty path!");

    auto itr = mValues.find(path[0].name());
    if (itr != mValues.end())
    {
       popKey(path);
        
       switch(itr->second.which())
       {
       case 0:
          return &boost::get<Value>(itr->second);
       case 1:
          return &boost::get<ValueGetter>(itr->second);
       default:
          assert(false);
       }
    }

    if (mAnonymous) {
      auto res = mAnonymous(path);
      if (res != ValueTag::Null)
        return &mAnonymous;
    }

    if (mParent == nullptr)
      return MapValuePtr{};

    return mParent->getPtr(path);
  }
  
  static bool hasIndexVariables(PathRef path)
  {
     for (auto& p : path)
     {
        if (p.isIndexVariable())
           return true;
     }
     
     return false;
  }

  Value getImpl(PathRef path) const {
    assert(!hasIndexVariables(path));
    auto ptr = getPtr(path);

    return getFromValues(ptr, path);
  }

public:
  Value get(PathRef path) const {
     if (hasIndexVariables(path))
     {
        Path pathCopy(path.begin(), path.end());
        for (auto& p : pathCopy)
        {
           if (!p.isIndexVariable())
              continue;
           
           auto idxPath = toPath(p.indexVariable().name);           
           p = Key{lex_cast<size_t>(get(idxPath).toString())};
        }
        return getImpl(pathCopy);
     }
     
     return getImpl(path);
  }

  void setLiquidValue(std::string name, Value value) {
    mValues[std::move(name)] = std::move(value);
  }

  template <typename T>
  void set(std::string name, const T &value,
           std::enable_if_t<!hasAccessor<T>, void **> = 0) {
    setLiquidValue(std::move(name), toValue(value));
  }

private:
  template <typename T> 
  static inline auto buildAccessorFunction(T &&value) {
    return [val = std::forward<T>(value)](PathRef path) {
      return Accessor<std::decay_t<T>>::get(val, path);
    };
  }

public:
  template <typename T>
  void set(std::string name, T &&value,
           std::enable_if_t<hasAccessor<std::decay_t<T>>, void **> = 0) {
    mValues[std::move(name)] = buildAccessorFunction(std::forward<T>(value));
  }

  void setLink(std::string name, string_view referencedPath) {
    auto p = toPath(referencedPath);
    setLink(std::move(name), p);
  }

  void setLink(std::string name, PathRef referencedPath) {
    if (referencedPath.empty())
      throw std::runtime_error("Can't handle empty path on set of link!");

    auto basePtr = getPtr(referencedPath);
    auto basePath = Path(referencedPath.begin(), referencedPath.end());
    switch(basePtr.which())
    {
       case 0:
       {
          auto& valPtr = boost::get<const Value*>(basePtr);
          if (valPtr == nullptr)
             mValues[std::move(name)] = Value{};
          else
             mValues[std::move(name)] =
               [this, val = *valPtr, basePath](PathRef subPath) {
                  auto p = basePath + subPath;
                  return getFromValues(&val, p);
               };
          break;
       }
       case 1:
       {
         mValues[std::move(name)] =
            [this, valGetter = *boost::get<const ValueGetter*>(basePtr), basePath](PathRef subPath) {
               auto p = basePath + subPath;
               return getFromValues(&valGetter, p);
            };
       }
    }
  }

  template <typename T> void setAnonymous(T &&value) {
    mAnonymous = buildAccessorFunction(std::forward<T>(value));
  }
};
}
