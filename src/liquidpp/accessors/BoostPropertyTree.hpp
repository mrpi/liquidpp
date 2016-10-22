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
            auto parent = propTree;

            while(auto key = popKey(path))
            {
               auto child = propTree.get_child_optional(key.name.to_string());
               if (!child)
               {
                  if (path.empty())
                  {
                     auto res = parent.template get_optional<std::string>(key.name.to_string());
                     if (res)
                        return *res;
                  }

                  return ValueTag::Null;
               }

               if (key.idx)
               {
                  size_t i = 0;
                  for (auto&& c : *child)
                  {
                     if (i++ == *key.idx)
                     {
                        parent = c.second;
                        break;
                     }
                  }
                  if (i <= *key.idx)
                     return ValueTag::OutOfRange;
                  continue;
               }
               else
                  parent = *child;
            }

            if (parent.empty())
            {
               auto res = parent.template get_value_optional<std::string>();
               if (res)
                  return *res;

               return ValueTag::Null;
            }
            else
               return RangeDefinition{parent.size()};
         };
      }
   };
}
