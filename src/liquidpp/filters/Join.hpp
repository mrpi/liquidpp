#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

namespace liquidpp
{
namespace filters
{

struct Join
{
   Value operator()(Value&& val, Value&& separator) const
   {
      if (!val.isRange())
         return std::move(val);

      auto sepVal = separator.toString();

      std::string res;

      auto& vals = val.range().inlineValues();
      bool first = true;
      for (auto& val : vals)
      {
         if (first)
            first = false;
         else
            res += sepVal;

         res += val;
      }

      return std::move(res);
   }
};

}
}
