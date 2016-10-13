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

   struct Key
   {
      string_view name;
      boost::optional<size_t> idx;

      explicit operator bool() const
      {
         return !name.empty();
      }
   };

   using Path = std::vector<Key>;

   inline Key popKey(string_view& path)
   {
      Key res;

      auto pos = path.find('.');
      if (pos == std::string::npos)
      {
         res.name = path;
         path.clear();
      }
      else
      {
         res.name = path.substr(0, pos);
         path.remove_prefix(pos + 1);
      }

      auto idxStart = res.name.find('[');
      if (idxStart != std::string::npos && res.name.back() == ']')
      {
         auto size = res.name.size();
         res.idx = boost::lexical_cast<size_t>(res.name.substr(idxStart + 1, size - idxStart - 2));
         res.name.remove_suffix(size - idxStart);
      }

      return res;
   }

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
       
       Value get(const string_view qualifiedPath) const
       {
          auto path = qualifiedPath;
          auto key = popKey(path);

          auto itr = mValues.find(key.name);
          if (itr != mValues.end())
          {
            if (key.idx)
            {
               if (itr->second.which() == 1)
                  return boost::get<ValueGetter>(itr->second)(boost::lexical_cast<std::string>(*key.idx));
            }
            else if (path.empty())
            {
               if (itr->second.which() == 0)
                  return boost::get<std::string>(itr->second);
            }
            else
            {
               if (itr->second.which() == 1)
                  return boost::get<ValueGetter>(itr->second)(path);
            }
          }
          
          if (mAnonymous)
          {
             auto res = mAnonymous(qualifiedPath);
             if (res)
                return res;
          }

          if (mParent)
             return mParent->get(qualifiedPath);
          
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
