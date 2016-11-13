#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

namespace liquidpp
{
namespace filters
{

struct Round
{
   Value operator()(Value&& val, Value&& arg1) const
   {
      std::ostringstream oss;
      oss << std::fixed << std::setprecision(arg1 ? arg1.integralValue() : 0);

      if (val.isFloatingPoint())
         oss << val.floatingPointValue();
      if (val.isIntegral())
         oss << val.integralValue();

      return oss.str();
   }
};

}
}
