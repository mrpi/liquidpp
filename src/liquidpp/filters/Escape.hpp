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

struct EscapeOnce : public Filter
{
   static bool isStartValidOfEscapeSequence(string_view s)
   {
      if (s.size() < 3)
         return false;

      if (std::isalpha(s[1]))
      {
         for (auto c : s.substr(2))
         {
            if (std::isalpha(c))
               continue;
            if (c == ';')
               return true;
            return false;
         }
      }
      else if (s[1] == '#')
      {
         size_t pos = 0;
         if (s[2] == 'x')
            pos = s.find_first_not_of("0123456789ABCDEFabcdef", 3);
         else
            pos = s.find_first_not_of("0123456789", 2);
         if (pos == std::string::npos)
            return false;
         return s[pos] == ';';
      }

      return false;
   }

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      auto sv = *val;

      std::string res;
      res.reserve(sv.size());

      while (!sv.empty())
      {
         auto&& c = sv[0];
         switch(c)
         {
            case '<':
               res += "&lt;";
               break;
            case '>':
               res += "&gt;";
               break;
            case '&':
               if (isStartValidOfEscapeSequence(sv))
                  res += c;
               else
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

         sv.remove_prefix(1);
      }

      return std::move(res);
   }
};

}
}