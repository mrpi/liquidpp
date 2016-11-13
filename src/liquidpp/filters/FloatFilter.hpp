#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

#include <cmath>

#include <boost/lexical_cast.hpp>

namespace liquidpp
{
namespace filters
{

template<typename Func>
struct FloatFilter
{
   Func func;

   Value operator()(Value&& val) const
   {
      if (val.isFloatingPoint())
         return toValue(func(val.floatingPointValue()));
      if (val.isIntegral())
         return std::move(val);

      auto sv = *val;
      if (!Expression::isInteger(sv) && Expression::isFloat(sv))
      {
         auto d = boost::lexical_cast<double>(sv);
         return toValue(func(d));
      }

      return std::move(val);
   }
};

template<typename Func>
auto makeFloatFilter(Func&& f)
{
   return FloatFilter<Func>{std::forward<Func>(f)};
}

}
}
