#pragma once

#include <string>
#include <utility>
#include <unordered_map>
#include <map>

#include <boost/variant/recursive_variant.hpp>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree_fwd.hpp>

namespace liquidpp
{
    using Value = boost::optional<std::string>;
    using ValueGetter = std::function<Value(const std::string&)>;

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
    
    template<typename Key, typename ValueT>
    struct ValueConverter<std::map<Key, ValueT>> : public std::true_type
    {
       template<typename T>
       static auto get(T&& map)
       {
          return [map = std::forward<T>(map)](const std::string& name) -> Value
          {
             auto itr = map.find(boost::lexical_cast<Key>(name));
             if (itr != map.end())
                return toValue(itr->second);
              return Value{};
          };
       }
    };
    
    template<typename ValueT>
    struct ValueConverter<std::vector<ValueT>> : public std::true_type
    {
       template<typename T>
       static auto get(T&& vec)
       {
          return [vec = std::forward<T>(vec)](const std::string& name) -> Value
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
          return [propTree = std::forward<T>(propTree)](const std::string& name) -> Value
          {
             return propTree.template get_optional<std::string>(name);
          };
       }
    };
       
    class Context
    {
    private:
       const Context* mParent{nullptr};
       using MapValue = boost::variant<std::string, ValueGetter>;
       std::unordered_map<std::string, MapValue> mValues;
       ValueGetter mAnonymous;
       
    public:
       Context() = default;
       
       explicit Context(const Context* parent)
         : mParent(parent)
       {          
       }
       
       static std::pair<std::string, std::string> splitEntry(const std::string& name)
       {
          auto sepPos = name.find('.');
          if (sepPos != std::string::npos)
             return {name.substr(0, sepPos), name.substr(sepPos+1)};

          sepPos = name.find('[');
          auto sepPos1 = name.find(']');
          if (sepPos != std::string::npos && sepPos1 != std::string::npos && sepPos < sepPos1)
             return {name.substr(0, sepPos), name.substr(sepPos+1, sepPos1-sepPos-1)};
          
          return {name, std::string{}};
       }
       
       Value get(const std::string& name) const
       {
          auto parts = splitEntry(name);
          if (parts.second.empty())
          {
            auto itr = mValues.find(parts.first);
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
