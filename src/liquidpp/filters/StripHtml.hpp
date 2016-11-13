#pragma once

#include "Filter.hpp"

namespace liquidpp
{
namespace filters
{

struct StripHtml
{
   static std::string stripX(string_view str, string_view start, string_view end)
   {
      std::string res;
      res.reserve(str.size());

      while (true)
      {
         auto pos = str.find(start);
         if (pos == std::string::npos)
         {
            res.append(str.data(), str.size());
            break;
         }

         res.append(str.data(), pos);
         str.remove_prefix(pos);

         auto endPos = str.find(end, start.size());
         if (endPos == std::string::npos)
         {
            res.append(str.data(), str.size());
            break;
         }

         str.remove_prefix(endPos + end.size());
      }

      return res;
   }

   Value operator()(Value&& val) const
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      std::string res = stripX(*val, "<script", "</script>");
      res = stripX(res, "<!--", "-->");
      res = stripX(res, "<style", "</style>");
      res = stripX(res, "<", ">");

      return std::move(res);
   }
};

}
}