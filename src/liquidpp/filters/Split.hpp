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
            r.push_back(ch.to_string());
         }
      }
      else
      {
         while(true)
         {
            auto pos = sv.find(separator);
            if (pos == std::string::npos)
            {
               r.push_back(sv.to_string());
               sv = string_view{};
               break;
            }

            r.push_back(sv.substr(0, pos).to_string());
            sv.remove_prefix(pos + separator.size());
         }
      }

      return RangeDefinition{std::move(r)};
   }
};

}
}
