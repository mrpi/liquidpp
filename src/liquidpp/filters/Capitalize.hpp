#pragma once

#include "Filter.hpp"

namespace liquidpp
{
namespace filters
{

struct Capitalize : public Filter
{
   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      auto sv = *val;

      std::locale loc;
      std::string res = sv.to_string();

      // TODO: No real UTF-8 handling (boost::locale or ICU required)
      if (!res.empty())
         res[0] = std::toupper(sv[0], loc);

      return std::move(res);
   }
};

}
}