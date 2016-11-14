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
        mMinOutputPer1024Loops{parent->mMinOutputPer1024Loops} {
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
  Value getFromValues(StorageT::const_iterator itr, PathRef path) const {
    auto key = popKey(path);

    if (itr->second.which() == 1) {
      auto &valueGetter = boost::get<ValueGetter>(itr->second);
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

    if (key.isIndex())
      return ValueTag::SubValue;

    if (!path.empty()) {
      if (path.size() == 1 && path[0] == "size")
        return toValue(boost::get<Value>(itr->second).toString().size());

      return ValueTag::SubValue;
    }

    return boost::get<Value>(itr->second).asReference();
  }

public:
  Value get(const PathRef path) const {
    if (path.empty())
      throw std::runtime_error("Can't handle empty path!");

    auto itr = mValues.find(path[0].name());
    if (itr != mValues.end())
      return getFromValues(itr, path);

    if (mAnonymous) {
      auto res = mAnonymous(path);
      if (res != ValueTag::Null)
        return res;
    }

    if (mParent == nullptr)
      return ValueTag::Null;

    return mParent->get(path);
  }

  void setLiquidValue(std::string name, Value value) {
    mValues[name] = std::move(value);
  }

  template <typename T>
  void set(std::string name, const T &value,
           std::enable_if_t<!hasAccessor<T>, void **> = 0) {
    setLiquidValue(std::move(name), toValue(value));
  }

private:
  template <typename T> static inline auto buildAccessorFunction(T &&value) {
    return [val = std::forward<T>(value)](PathRef path) {
      return Accessor<std::decay_t<T>>::get(val, path);
    };
  }

public:
  template <typename T>
  void set(std::string name, T &&value,
           std::enable_if_t<hasAccessor<std::decay_t<T>>, void **> = 0) {
    mValues[name] = buildAccessorFunction(std::forward<T>(value));
  }

  void setLink(std::string name, string_view referencedPath) {
    auto p = toPath(referencedPath);
    setLink(name, p);
  }

  void setLink(std::string name, PathRef referencedPath) {
    auto callRef =
        [ this, basePath = Path(referencedPath.begin(), referencedPath.end()) ](
            PathRef subPath) {
      auto p = basePath + subPath;
      return get(p);
    };
    mValues[name] = std::move(callRef);
  }

  template <typename T> void setAnonymous(T &&value) {
    mAnonymous = buildAccessorFunction(std::forward<T>(value));
  }
};
}
