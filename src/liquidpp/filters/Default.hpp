#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

namespace liquidpp
{
namespace filters
{

struct Default : public Filter
{
   Expression::Token defaultValue;

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (val)
      {
         auto str = val.toString();
         if (!str.empty())
            return std::move(val);
      }

      return Expression::value(c, defaultValue);
   }

   virtual void addAttribute(string_view sv) override final
   {
      defaultValue = Expression::toToken(sv);
   }
};

}
}
