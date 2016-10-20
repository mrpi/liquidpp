#include "Tag.hpp"

#include <tuple>

namespace liquidpp
{

   bool Tag::operator==(const Tag& other) const {
      auto tup = [](auto& var){ return std::tie(var.name, var.value); };
      return tup(*this) == tup(other);
   }

}