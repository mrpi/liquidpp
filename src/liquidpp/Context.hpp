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

public:
   Context() = default;

   explicit Context(const Context* parent)
      : mParent(parent), mDocumentScopeContext(this) {
      assert(mParent->mDocumentScopeContext == nullptr);
   }

   explicit Context(Context* parent)
      : mParent(parent), mDocumentScopeContext(mParent->mDocumentScopeContext) {
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
               Value parent;
               if (path == "size")
                  parent = boost::get<ValueGetter>(itr->second)(key.idx, path);
               else if (path.size() > 5 && path.substr(path.size()-5) == ".size")
                  parent = boost::get<ValueGetter>(itr->second)(key.idx, path.substr(0, path.size()-5));

               if (parent)
                  return toValue(parent.size());
            }
            return std::move(res);
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
