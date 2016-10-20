#pragma once

#include "Filter.hpp"

namespace liquidpp
{
namespace filters
{

struct StripNewlines : public Filter
{
   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      auto sv = *val;

      std::string res;
      res.reserve(sv.size());

      for (auto c : sv)
      {
         switch(c)
         {
            case '\r':
            case '\n':
               break;
            default:
               res += c;
               break;
         }
      }

      return std::move(res);
   }
};

}
}