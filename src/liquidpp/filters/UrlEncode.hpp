#pragma once

#include "Filter.hpp"

namespace liquidpp
{
namespace filters
{

struct UrlEncode
{
   Value operator()(Value&& val) const
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      auto sv = *val;

      // TODO: Get rid of ostringstream (too slow)
      std::ostringstream escaped;
      escaped.fill('0');
      escaped << std::hex;

      for (char c : sv)
      {
         if (isalnum(c))
         {
            escaped << c;
            continue;
         }

         switch(c)
         {
            case '-':
            case '_':
            case '.':
            case '~':
               escaped << c;
               break;
            case ' ':
               escaped << '+';
               break;
            default:
            {
               auto charNum = static_cast<int>(static_cast<unsigned char>(c));
               escaped << std::uppercase << '%' << std::setw(2) << charNum;
               escaped << std::nouppercase; // reset
               break;
            }
         }
      }

      return escaped.str();
   }
};

}
}