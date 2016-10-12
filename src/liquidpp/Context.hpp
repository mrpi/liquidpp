#pragma once

#include <string>
#include <utility>
#include <unordered_map>
#include <map>

#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree_fwd.hpp>

#include "config.h"

namespace liquidpp
{
    using Value = boost::optional<std::string>;
    using ValueGetter = std::function<Value(string_view)>;

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
            return [map = std::forward<T>(map)](string_view name) -> Value
            {
               auto itr = map.find(boost::lexical_cast<KeyT>(name));
               if (itr != map.end())
                  return toValue(itr->second);
               return Value{};
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
          return [vec = std::forward<T>(vec)](string_view name) -> Value
          {
             auto idx = boost::lexical_cast<size_t>(name);
             if (idx < vec.size())
                return toValue(vec[idx]);
             return Value{};
          };
       }
    };
    
    template<>
    struct ValueConverter<boost::property_tree::ptree> : public std::true_type
    {
       template<typename T>
       static auto get(T&& propTree)
       {
          return [propTree = std::forward<T>(propTree)](string_view name) -> Value
          {
             return propTree.template get_optional<std::string>(name.to_string());
          };
       }
    };
       
    class Context
    {
    private:
       const Context* mParent{nullptr};
       using MapValue = boost::variant<std::string, ValueGetter>;
       using StorageT = std::map<std::string, MapValue, std::less<void>>;
       StorageT mValues;
       ValueGetter mAnonymous;
       
    public:
       Context() = default;
       
       explicit Context(const Context* parent)
         : mParent(parent)
       {          
       }
       
       Context(std::initializer_list<StorageT::value_type> entries)
        : mValues(entries)
       {
       }
       
       static std::pair<string_view, string_view> splitEntry(string_view name)
       {
          auto sepPos = name.find('.');
          if (sepPos != std::string::npos)
             return {name.substr(0, sepPos), name.substr(sepPos+1)};

          sepPos = name.find('[');
          auto sepPos1 = name.find(']');
          if (sepPos != std::string::npos && sepPos1 != std::string::npos && sepPos < sepPos1)
             return {name.substr(0, sepPos), name.substr(sepPos+1, sepPos1-sepPos-1)};
          
          return {name, string_view{}};
       }
       
       Value get(string_view name) const
       {
          auto parts = splitEntry(name);
          if (parts.second.empty())
          {
             std::string x;
             name < x;
             x < name;
             
            auto itr = mValues.find(name);
            if (itr != mValues.end() && itr->second.which() == 0)
               return boost::get<std::string>(itr->second);
          }
          else
          {
             auto itr = mValues.find(parts.first);
             if (itr != mValues.end() && itr->second.which() == 1)
                return boost::get<ValueGetter>(itr->second)(parts.second);
          }
          
          if (mAnonymous)
          {
             auto res = mAnonymous(name);
             if (res)
                return res;
          }

          if (mParent)
             return mParent->get(name);
          
          return Value{};             
       }
       
       template<typename T, typename = decltype(toValue(std::declval<T>))>
       void set(std::string name, const T& value, std::enable_if_t<!hasValueConverter<T>, void**> = 0)
       {
          mValues.insert({std::move(name), toValue(value)});
       }
       
       template<typename T>
       void set(std::string name, T&& value, std::enable_if_t<hasValueConverter<std::decay_t<T>>, void**> = 0)
       {
          mValues.insert({std::move(name), ValueConverter<std::decay_t<T>>::get(std::forward<T>(value))});
       }
       
       template<typename T>
       void setAnonymous(T&& value)
       {
          mAnonymous = ValueConverter<std::decay_t<T>>::get(std::forward<T>(value));
       }
    };
}
