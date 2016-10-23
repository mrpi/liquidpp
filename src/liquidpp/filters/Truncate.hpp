#pragma once

#include "../Expression.hpp"
#include "Filter.hpp"

namespace liquidpp
{
namespace filters
{

struct Truncate : public Filter
{
   Expression::Token maxCountToken;
   Expression::Token ellipsisToken;

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      auto sv = *val;

      size_t maxCount = Expression::value(c, maxCountToken).integralValue();
      if (sv.size() <= maxCount)
         return std::move(val);

      std::string ellips = "...";
      if (!(ellipsisToken == Expression::Token{}))
         ellips = Expression::value(c, ellipsisToken).toString();

      return sv.substr(0, maxCount - ellips.size()).to_string() + ellips;
   }

   virtual void addAttribute(string_view sv) override final
   {
      if (maxCountToken == Expression::Token{})
         maxCountToken = Expression::toToken(sv);
      else if (ellipsisToken == Expression::Token{})
         ellipsisToken = Expression::toToken(sv);
      else
         throw Exception("Too many attributes!", sv);
   }
};

}
}