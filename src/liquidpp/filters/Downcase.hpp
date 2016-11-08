#pragma once

#include "Filter.hpp"
#include "../Context.hpp"

#include <boost/algorithm/string.hpp>

namespace liquidpp
{
namespace filters
{

struct Downcase : public Filter
{
   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      auto sv = *val;
      std::string res;
      res.reserve(sv.size());
      auto&& conv = std::use_facet<std::ctype<wchar_t>>(c.locale());
      
      while (auto ch = utf8::popU32Char(sv))
      {
         auto wChar = static_cast<wchar_t>(*ch);
         auto transfChar = conv.tolower(wChar);
         
         if (transfChar != wChar)
            utf8::append(res, transfChar);
         else
            utf8::append(res, *ch);
      }
      
      return res;
   }
};

}
}
