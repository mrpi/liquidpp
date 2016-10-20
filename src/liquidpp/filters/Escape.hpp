#pragma once

#include "Filter.hpp"

namespace liquidpp
{
namespace filters
{

struct Escape : public Filter
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
            case '<':
               res += "&lt;";
               break;
            case '>':
               res += "&gt;";
               break;
            case '&':
               res += "&amp;";
               break;
            case '\"':
               res += "&quot;";
               break;
            case '\'':
               res += "&#39;";
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