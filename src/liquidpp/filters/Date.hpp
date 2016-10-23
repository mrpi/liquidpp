#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

namespace liquidpp
{
namespace filters
{

struct Date : public Filter
{
   Expression::Token dateFormat;

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      return std::string{"FIXME: 'date' filter is currently a dummy!"};
   }

   virtual void addAttribute(string_view sv) override final
   {
      dateFormat = Expression::toToken(sv);
   }
};

}
}
