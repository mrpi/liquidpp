#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

#include <boost/algorithm/string.hpp>

namespace liquidpp
{
namespace filters
{

struct Remove : public Filter
{
   Expression::Token toRemoveToken;

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      auto str = val.toString();
      auto toRemove = Expression::value(c, toRemoveToken).toString();

      boost::algorithm::replace_all(str, toRemove, "");
      return std::move(str);
   }

   virtual void addAttribute(string_view sv)
   {
      toRemoveToken = Expression::toToken(sv);
   }
};

}
}