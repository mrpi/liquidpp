#pragma once

#include <boost/property_tree/ptree_fwd.hpp>

namespace liquidpp
{
   template<>
   struct ValueConverter<boost::property_tree::ptree> : public std::true_type
   {
      template<typename T>
      static auto get(T&& propTree)
      {
         return [propTree = std::forward<T>(propTree)](OptIndex idx, string_view path) -> Value
         {
            auto res = propTree.template get_optional<std::string>(path.to_string());
            if (res)
               return *res;
            return ValueTag::Null;
         };
      }
   };
}
