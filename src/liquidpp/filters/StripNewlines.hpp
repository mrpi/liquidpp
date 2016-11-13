#pragma once

#include "Filter.hpp"

namespace liquidpp
{
namespace filters
{

struct StripNewlines
{
   Value operator()(Value&& val) const
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