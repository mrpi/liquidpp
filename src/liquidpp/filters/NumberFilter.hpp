#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

namespace liquidpp
{
namespace filters
{

template<typename Func>
struct NumberFilter : public Filter
{
   Func func;

   template<typename Func1>
   NumberFilter(Func1&& f)
    : func(std::forward<Func1>(f))
   {
   }

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (val.isFloatingPoint())
         return toValue(func(val.floatingPointValue()));
      if (val.isIntegral())
         return toValue(func(val.integralValue()));

      auto sv = *val;
      if (Expression::isInteger(sv))
      {
         auto i = boost::lexical_cast<std::intmax_t>(sv);
         return toValue(func(i));
      }

      if (Expression::isFloat(sv))
      {
         auto d = boost::lexical_cast<double>(sv);
         return toValue(func(d));
      }

      return std::move(val);
   }
};

template<typename Func>
auto makeNumberFilter(Func&& f)
{
   return std::make_shared<NumberFilter<Func>>(std::forward<Func>(f));
}

template<typename Func>
struct NumberFilter1Arg : public Filter
{
   Func func;
   Expression::Token token;

   template<typename Func1>
   NumberFilter1Arg(Func1&& f)
      : func(std::forward<Func1>(f))
   {
   }

   template<typename V, typename Arg1>
   Value apply(V v, Arg1 arg1) const
   {
      throw std::runtime_error("Can't execute filter with given argument!");
   }

   template<typename Arg1, typename = decltype(std::declval<double>() / std::declval<Arg1>())>
   Value apply(Value&& val, Arg1 arg1) const
   {
      if (val.isFloatingPoint())
         return toValue(func(val.floatingPointValue(), arg1));
      if (val.isIntegral())
         return toValue(func(val.integralValue(), arg1));

      auto sv = *val;
      if (Expression::isInteger(sv))
      {
         auto i = boost::lexical_cast<std::intmax_t>(sv);
         return toValue(func(i, arg1));
      }

      if (Expression::isFloat(sv))
      {
         auto d = boost::lexical_cast<double>(sv);
         return toValue(func(d, arg1));
      }

      return std::move(val);
   }

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      auto arg1 = Expression::value(c, token);
      if (arg1.isFloatingPoint())
         return apply(std::move(val), arg1.floatingPointValue());
      if (arg1.isIntegral())
         return apply(std::move(val), arg1.integralValue());

      return apply(std::move(val), *val);
   }

   virtual void addAttribute(string_view sv)
   {
      token = Expression::toToken(sv);
   }
};

template<typename Func>
auto makeNumberFilter1Arg(Func&& f)
{
   return std::make_shared<NumberFilter1Arg<Func>>(std::forward<Func>(f));
}

}
}
