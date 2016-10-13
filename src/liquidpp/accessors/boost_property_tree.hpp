

#include <boost/property_tree/ptree_fwd.hpp>

namespace liquidpp
{
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
}
