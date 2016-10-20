#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

#include <boost/locale/conversion.hpp>

namespace liquidpp
{
namespace filters
{

struct Append : public Filter
{
   Expression::Token toAppend;

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      return val.toString() + Expression::value(c, toAppend).toString();
   }

   virtual void addAttribute(string_view sv) override final
   {
      toAppend = Expression::toToken(sv);
   }
};

}
}
