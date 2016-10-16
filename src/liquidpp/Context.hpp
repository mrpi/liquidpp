#pragma once

#include <string>
#include <utility>
#include <unordered_map>
#include <map>

#include <boost/variant/recursive_variant.hpp>
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
   using MapValue = boost::variant<std::string, ValueGetter>;
   using StorageT = std::map<std::string, MapValue, std::less<void>>;
   StorageT mValues;
   ValueGetter mAnonymous;

public:
   Context() = default;

   explicit Context(const Context* parent)
      : mParent(parent) {
   }

   Context(std::initializer_list<StorageT::value_type> entries)
      : mValues(entries) {
   }

   Value get(const string_view qualifiedPath) const {
      auto path = qualifiedPath;
      auto key = popKey(path);

      auto itr = mValues.find(key.name);
      if (itr != mValues.end()) {
         if (itr->second.which() == 1)
            return boost::get<ValueGetter>(itr->second)(key.idx, path);

         if (key.idx || !path.empty())
            return ValueTag::SubValue;

         return boost::get<std::string>(itr->second);
      }

      if (mAnonymous) {
         auto res = mAnonymous(boost::none, qualifiedPath);
         if (res || boost::get<ValueTag>(res) != ValueTag::Null)
            return res;
      }

      if (mParent)
         return mParent->get(qualifiedPath);

      return ValueTag::Null;
   }

   template<typename T, typename = decltype(toValue(std::declval<T>))>
   void set(std::string name, const T& value, std::enable_if_t<!hasValueConverter<T>, void**> = 0) {
      mValues.insert({std::move(name), toValue(value)});
   }

   template<typename T>
   void set(std::string name, T&& value, std::enable_if_t<hasValueConverter<std::decay_t<T>>, void**> = 0) {
      mValues.insert({std::move(name), ValueConverter<std::decay_t<T>>::get(std::forward<T>(value))});
   }

   void setReference(std::string name, string_view referencedPath)
   {
      auto callRef = [this, referencedPath](OptIndex idx, string_view subPath)
      {
         return get(referencedPath.to_string() + '.' + subPath.to_string());
      };
      mValues.insert({std::move(name), std::move(callRef)});
   }

   template<typename T>
   void setAnonymous(T&& value) {
      mAnonymous = ValueConverter<std::decay_t<T>>::get(std::forward<T>(value));
   }
};
}
