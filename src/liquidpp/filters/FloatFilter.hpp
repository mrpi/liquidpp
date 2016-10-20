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
struct FloatFilter : public Filter
{
   Func func;

   template<typename Func1>
   FloatFilter(Func1&& f)
    : func(std::forward<Func1>(f))
   {
   }

   virtual Value operator()(Context& c, Value&& val) const override final
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
   return std::make_shared<FloatFilter<Func>>(std::forward<Func>(f));
}

}
}
