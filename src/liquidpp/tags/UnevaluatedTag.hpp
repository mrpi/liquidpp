#pragma once

#include "Tag.hpp"
#include "../Expression.hpp"
#include "../Exception.hpp"

namespace liquidpp
{

struct UnevaluatedTag : public Tag {
   UnevaluatedTag() = default;

   UnevaluatedTag(string_view data)
   {
      data.remove_prefix(data[2] == '-' ? 3 : 2);
      data.remove_suffix(data[data.size() - 3] == '-' ? 3 : 2);

      auto tokens = Expression::splitTokens(data);
      if (tokens.empty())
         throw Exception("Tag definition without tag name!", data);
      name = tokens[0];
      if (tokens.size() > 1)
      {
         const char* start = &tokens[1].front();
         const char* end = &*tokens.back().end();
         size_t len = end - start;
         value = string_view{start, len};
      }
   }

   void render(Context& context, std::string& out) const override final {
   }
};

};
