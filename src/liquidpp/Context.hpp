#pragma once

#include <string>
#include <utility>
#include <unordered_map>
#include <map>

#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>

#include "config.h"

#include "Key.hpp"
#include "ValueConverter.hpp"

#include "accessors/Accessors.hpp"

namespace liquidpp {
class Context {
private:
   const Context* mParent{nullptr};
   Context* mDocumentScopeContext{nullptr};
   using MapValue = boost::variant<Value, ValueGetter>;
   using StorageT = std::map<std::string, MapValue, std::less<void>>;
   StorageT mValues;
   ValueGetter mAnonymous;

   size_t mMaxOutputSize{8*1024*1024};
   size_t mMinOutputPer1024Loops{mMaxOutputSize / 4};
   size_t mRecursiveDepth{0};

public:
   Context() = default;

   explicit Context(const Context* parent)
      : mParent(parent),
        mDocumentScopeContext(this),
        mMaxOutputSize(parent->mMaxOutputSize),
        mMinOutputPer1024Loops{parent->mMinOutputPer1024Loops} {
      assert(mParent->mDocumentScopeContext == nullptr);
   }

   explicit Context(Context* parent)
      : mParent(parent),
        mDocumentScopeContext(mParent->mDocumentScopeContext),
        mMaxOutputSize(parent->mMaxOutputSize),
        mMinOutputPer1024Loops{parent->mMinOutputPer1024Loops} {
      assert(mDocumentScopeContext != nullptr);
   }

   Context(std::initializer_list<StorageT::value_type> entries)
      : mValues(entries) {
   }

   Context& documentScopeContext()
   {
      assert(mDocumentScopeContext);
      return *mDocumentScopeContext;
   }

   size_t maxOutputSize() const
   {
      return mMaxOutputSize;
   }

   void setMaxOutputSize(size_t val)
   {
      mMaxOutputSize = val;
   }

   size_t minOutputPer1024Loops() const
   {
      return mMinOutputPer1024Loops;
   }

   void setMinOutputPer1024Loops(size_t val)
   {
      mMinOutputPer1024Loops = val;
   }

   size_t& recursiveDepth()
   {
      return mRecursiveDepth;
   }

   Value get(const string_view qualifiedPath) const {
      auto path = qualifiedPath;
      auto key = popKey(path);

      auto itr = mValues.find(key.name);
      if (itr != mValues.end()) {
         if (itr->second.which() == 1)
         {
            auto res = boost::get<ValueGetter>(itr->second)(key.idx, path);
            if (res == ValueTag::SubValue)
            {
               auto lastKey = popLastKey(path);

               if (lastKey == "size")
               {
                  Value parent = boost::get<ValueGetter>(itr->second)(key.idx, path);
                  return toValue(parent.size());
               }

               if (lastKey == "first" || lastKey == "last")
               {
                  Value parent = boost::get<ValueGetter>(itr->second)(key.idx, path);
                  if (parent.isRange())
                  {
                     auto& range = parent.range();
                     auto size = range.size();
                     if (size == 0)
                        return ValueTag::Null;
                     auto idx = range.index(lastKey == "last" ? size-1 : 0);
                     if (!key.idx && path.empty())
                        return boost::get<ValueGetter>(itr->second)(idx, path);
                     return boost::get<ValueGetter>(itr->second)(key.idx, path.to_string() + '[' + boost::lexical_cast<std::string>(idx) + ']');
                  }
               }
            }
            return res;
         }

         if (key.idx)
            return ValueTag::SubValue;

         if (!path.empty())
         {
            if (path == "size")
               return toValue(boost::get<Value>(itr->second).toString().size());

            return ValueTag::SubValue;
         }

         return boost::get<Value>(itr->second);
      }

      if (mAnonymous) {
         auto res = mAnonymous(boost::none, qualifiedPath);
         if (res || res != ValueTag::Null)
            return res;
      }

      if (mParent)
         return mParent->get(qualifiedPath);

      return ValueTag::Null;
   }

   void setLiquidValue(std::string name, Value value) {
      mValues[name] = std::move(value);
   }

   template<typename T>
   void set(std::string name, const T& value, std::enable_if_t<!hasValueConverter<T>, void**> = 0) {
      setLiquidValue(std::move(name), toValue(value));
   }

   template<typename T>
   void set(std::string name, T&& value, std::enable_if_t<hasValueConverter<std::decay_t<T>>, void**> = 0) {
      mValues[name] = ValueConverter<std::decay_t<T>>::get(std::forward<T>(value));
   }

   void setLink(std::string name, string_view referencedPath)
   {
      auto callRef = [this, referencedPath](OptIndex idx, string_view subPath)
      {
         auto path = referencedPath.to_string();
         if (idx)
            path += "[" + boost::lexical_cast<std::string>(*idx) + "]";
         if (!subPath.empty())
         {
            path += '.';
            path.append(subPath.data(), subPath.size());
         }
         return get(path);
      };
      mValues[name] = std::move(callRef);
   }

   template<typename T>
   void setAnonymous(T&& value) {
      mAnonymous = ValueConverter<std::decay_t<T>>::get(std::forward<T>(value));
   }
};
}
