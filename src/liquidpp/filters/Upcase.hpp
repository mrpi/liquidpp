#pragma once

#include "Filter.hpp"
#include "../Context.hpp"

#include <boost/locale.hpp>

namespace liquidpp
{
namespace filters
{

struct Upcase : public Filter
{
   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      return boost::locale::to_upper(val.toString(), c.locale());
   }
};

}
}
