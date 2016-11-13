#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

namespace liquidpp
{
namespace filters
{

struct Reverse
{
   Value operator()(Value&& val) const
   {
      if (!val.isRange())
         return std::move(val);

      auto& vals = val.range().inlineValues();
      std::reverse(vals.begin(), vals.end());

      return std::move(val);
   }
};

}
}