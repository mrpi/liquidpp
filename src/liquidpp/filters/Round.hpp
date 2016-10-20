#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

namespace liquidpp
{
namespace filters
{

struct Round : public Filter
{
   Expression::Token token{toValue(0)};

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      auto arg1 = Expression::value(c, token);
      std::ostringstream oss;
      oss << std::fixed << std::setprecision(arg1.integralValue());

      if (val.isFloatingPoint())
         oss << val.floatingPointValue();
      if (val.isIntegral())
         oss << val.integralValue();

      return oss.str();
   }

   virtual void addAttribute(string_view sv)
   {
      token = Expression::toToken(sv);
   }
};

}
}
