#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

namespace liquidpp
{
namespace filters
{

struct Join : public Filter
{
   Expression::Token separatorToken;

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isRange())
         return std::move(val);

      auto sepVal = Expression::value(c, separatorToken).toString();

      std::string res;

      auto& vals = val.range().inlineValues();
      bool first = true;
      for (auto& val : vals)
      {
         if (first)
            first = false;
         else
            res += sepVal;

         res += val;
      }

      return std::move(res);
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