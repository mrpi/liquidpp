#pragma once

#include "Filter.hpp"

namespace liquidpp
{
namespace filters
{

struct NewlineToBr
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
               break;
            case '\n':
               res += "<br />\n";
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
