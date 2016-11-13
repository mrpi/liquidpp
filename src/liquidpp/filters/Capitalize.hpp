#pragma once

#include "Filter.hpp"
#include "../Context.hpp"

#include <boost/algorithm/string.hpp>

namespace liquidpp
{
namespace filters
{

struct Capitalize
{
   struct Upper : public std::ctype<wchar_t>
   {
      wchar_t operator()(wchar_t c)
      {
         return toupper(c);
      }
   };
   
   Value operator()(Context& c, Value&& val) const
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      auto sv = *val;
      const auto svSize = sv.size();
      
      auto firstChar = utf8::popU32Char(sv);
      if (firstChar)
      {
         auto lowerChar = static_cast<wchar_t>(*firstChar);
         auto upperChar = std::use_facet<std::ctype<wchar_t>>(c.locale()).toupper(lowerChar);
         if (lowerChar != upperChar)
         {
            std::string res;
            res.reserve(svSize);
            //res.append(upperChar.begin(), upperChar.end());
            utf8::append(res, upperChar);
            res.append(sv.data(), sv.size());
            return res;
         }
      }

      return std::move(val);
   }
};

}
}
