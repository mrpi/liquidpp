#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

namespace liquidpp
{
namespace filters
{

struct Default
{
   Value operator()(Value&& val, Value&& defaultValue) const
   {
      if (val)
      {
         auto str = val.toString();
         if (!str.empty())
            return std::move(val);
      }

      return defaultValue;
   }
};

}
}
