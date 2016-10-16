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
      Value() = default;
      Value(const Value&) = default;
      Value(Value&&) = default;
      Value& operator=(const Value&) = default;
      Value& operator=(Value&&) = default;

      Value(ValueTag tag)
       : ValueBase(tag)
      {}

      Value(const std::string& v)
         : ValueBase(v)
      {}

      Value(std::string&& v)
         : ValueBase(std::move(v))
      {}

      Value(const ValueBase& vb)
         : ValueBase(vb)
      {}

      Value(ValueBase&& vb)
         : ValueBase(std::move(vb))
      {}

      Value& operator=(const ValueBase& vb)
      {
         static_cast<ValueBase&>(*this) = vb;
         return *this;
      }

      Value& operator=(ValueBase&& vb)
      {
         static_cast<ValueBase&>(*this) = std::move(vb);
         return *this;
      }

      explicit operator bool() const
      {
         return which() == 0;
      }

      auto operator*() const
      {
         return boost::get<std::string>(*this);
      }

      bool operator==(ValueTag tag) const
      {
         if (*this)
            return false;
         return boost::get<ValueTag>(*this) == tag;
      }

      bool operator!=(ValueTag tag) const
      {
         return !(*this == tag);
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
