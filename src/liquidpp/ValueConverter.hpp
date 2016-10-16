#pragma once

#include <string>
#include <functional>
#include <unordered_map>

#include <boost/optional.hpp>

#include "config.h"

namespace liquidpp
{
   enum class ValueTag
   {
      Object,
      Range,
      Null,
      OutOfRange,
      SubValue
   };

   using ValueBase = boost::variant<std::string, ValueTag>;
   struct Value : public ValueBase
   {
      using ValueBase::ValueBase;

      explicit operator bool() const
      {
         return which() == 0;
      }

      auto operator*() const
      {
         return boost::get<std::string>(*this);
      }
   };

   using ValueGetter = std::function<Value(OptIndex, string_view)>;

    template<typename T>
    struct ValueConverter : public std::false_type
    {};
    
    template<typename T>
    constexpr bool hasValueConverter = ValueConverter<T>::value;
           
    template<typename T, typename = decltype(boost::lexical_cast<std::string>(std::declval<T>()))>
    std::string toValue(const T& value, std::enable_if_t<!hasValueConverter<T>, void**> = 0)
    {
       return boost::lexical_cast<std::string>(value);
    }
           
    template<typename T>
    std::string toValue(const T& value, std::enable_if_t<hasValueConverter<T>, void**> = 0)
    {
       return ValueConverter<T>::get(value);
    }
    
    namespace impl
    {
       template<typename KeyT, typename ValueT>
       struct AssociativeContainerConverter : public std::true_type
       {
         template<typename T>
         static auto get(T&& map)
         {
            return [map = std::forward<T>(map)](OptIndex idx, string_view path) -> Value
            {
               auto itr = map.find(boost::lexical_cast<KeyT>(path));
               if (itr != map.end())
                  return toValue(itr->second);
               return ValueTag::Null;
            };
         }
       };       
    }
    
    template<typename KeyT, typename ValueT>
    struct ValueConverter<std::map<KeyT, ValueT>> : public impl::AssociativeContainerConverter<KeyT, ValueT>
    {};
    
    template<typename KeyT, typename ValueT>
    struct ValueConverter<std::unordered_map<KeyT, ValueT>> : public impl::AssociativeContainerConverter<KeyT, ValueT>
    {};
    
    template<typename ValueT>
    struct ValueConverter<std::vector<ValueT>> : public std::true_type
    {
       template<typename T>
       static auto get(T&& vec)
       {
          return [vec = std::forward<T>(vec)](OptIndex idx, string_view path) -> Value
          {
             if (!idx)
                return ValueTag::Range;
             if (*idx < vec.size())
                return toValue(vec[*idx]);
             return ValueTag::OutOfRange;
          };
       }
    };
}
