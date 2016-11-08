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
