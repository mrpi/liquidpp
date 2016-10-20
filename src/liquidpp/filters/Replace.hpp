#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

#include <boost/algorithm/string.hpp>

namespace liquidpp
{
namespace filters
{

struct Replace : public Filter
{
   Expression::Token toRemoveToken;
   Expression::Token replacementToken;

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      auto str = val.toString();
      auto toRemove = Expression::value(c, toRemoveToken).toString();
      auto replacement = Expression::value(c, replacementToken).toString();

      boost::algorithm::replace_all(str, toRemove, replacement);
      return std::move(str);
   }

   virtual void addAttribute(string_view sv)
   {
      if (toRemoveToken == Expression::Token{})
         toRemoveToken = Expression::toToken(sv);
      else if (replacementToken == Expression::Token{})
         replacementToken = Expression::toToken(sv);
      else
         throw Exception("Too many arguments for filter!", sv);
   }
};

}
}
