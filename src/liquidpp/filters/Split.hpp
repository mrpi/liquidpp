#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

namespace liquidpp
{
namespace filters
{

struct Split
{
   Expression::Token separatorToken;

   Value operator()(Value&& val, Value&& sepVal) const
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      auto separator = *sepVal;

      RangeDefinition::InlineValues r;
      auto sv = *val;
      
      if (separator.empty())
      {
         r.reserve(sv.size());
         while(true)
         {
            auto ch = utf8::popU8Char(sv);
            if (ch.empty())
               break;
            r.push_back(to_string(ch));
         }
      }
      else
      {
         while(true)
         {
            auto pos = sv.find(separator);
            if (pos == std::string::npos)
            {
               r.push_back(to_string(sv));
               sv = string_view{};
               break;
            }

            r.push_back(to_string(sv.substr(0, pos)));
            sv.remove_prefix(pos + separator.size());
         }
      }

      return RangeDefinition{std::move(r)};
   }
};

}
}
