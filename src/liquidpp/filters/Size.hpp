#pragma once

#include "Filter.hpp"

namespace liquidpp
{
namespace filters
{

struct Size : public Filter
{
   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isStringViewRepresentable())
         return toValue(0);

      auto sv = *val;
      return toValue(sv.size());
   }
};

}
}
