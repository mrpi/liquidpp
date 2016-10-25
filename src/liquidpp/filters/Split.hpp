#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

namespace liquidpp
{
namespace filters
{

struct Split : public Filter
{
   Expression::Token separatorToken;

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      auto sepVal = Expression::value(c, separatorToken);
      auto separator = *sepVal;

      RangeDefinition::InlineValues r;
      auto sv = *val;
      while(true)
      {
         auto pos = sv.find(separator);
         if (pos == std::string::npos)
         {
            r.push_back(sv.to_string());
            sv = string_view{};
            break;
         }

         // with empty separator pos can be 0
         if (pos == 0)
         {
            // TODO: Don't split utf8 multibyte charaters
            r.push_back(sv.substr(0, 1).to_string());
            sv.remove_prefix(1);
            if (sv.empty())
               break;
         }

         r.push_back(sv.substr(0, pos).to_string());
         sv.remove_prefix(pos + separator.size());
      }

      return RangeDefinition{std::move(r)};
   }

   virtual void addAttribute(string_view sv) override final
   {
      if (separatorToken == Expression::Token{})
         separatorToken = Expression::toToken(sv);
      else
         throw Exception("Too many attributes!", sv);
   }
};

}
}