#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

namespace liquidpp
{
namespace filters
{

struct Prepend : public Filter
{
   Expression::Token prefix;

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      auto str = val.toString();
      return Expression::value(c, prefix).toString() + str;
   }

   virtual void addAttribute(string_view sv)
   {
      prefix = Expression::toToken(sv);
   }
};

}
}
