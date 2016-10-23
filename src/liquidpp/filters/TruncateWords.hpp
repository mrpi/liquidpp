#pragma once

#include "../Expression.hpp"
#include "Filter.hpp"

namespace liquidpp
{
namespace filters
{

struct TruncateWords : public Filter
{
   Expression::Token maxWordCountToken;
   Expression::Token ellipsisToken;

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      auto sv = *val;

      std::string res;
      res.reserve(sv.size());

      size_t wordCount = 0;
      size_t maxWordCount = Expression::value(c, maxWordCountToken).integralValue();
      std::string ellips = "...";
      if (ellipsisToken != Expression::Token{})
         ellips = Expression::value(c, ellipsisToken).toString();

      bool lastWasWhitespace = true;
      for (auto c : sv)
      {
         if (Expression::isWhitespace(c))
         {
            if (wordCount == maxWordCount)
               break;
            lastWasWhitespace = true;
         }
         else
         {
            if (lastWasWhitespace)
            {
               wordCount++;
               lastWasWhitespace = false;
            }
         }
         res += c;
      }

      if (res.size() < sv.size())
         res += ellips;

      return std::move(res);
   }

   virtual void addAttribute(string_view sv) override final
   {
      if (maxWordCountToken == Expression::Token{})
         maxWordCountToken = Expression::toToken(sv);
      else if (ellipsisToken == Expression::Token{})
         ellipsisToken = Expression::toToken(sv);
      else
         throw Exception("Too many attributes!", sv);
   }
};

}
}