#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

#include <boost/date_time/posix_time/conversion.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace liquidpp
{
namespace filters
{

struct Date : public Filter
{
   Expression::Token dateFormat;

   static bool parse(std::tm& t, const std::string& str, const char* pattern)
   {
      std::istringstream iss(str);
      iss >> std::get_time(&t, pattern);
      if (iss.fail())
         return false;
      return true;
   }

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      std::tm t = {};

      auto inDateStr = val.toString();
      if (inDateStr == "now" || inDateStr == "today")
      {
         t = boost::posix_time::to_tm(boost::posix_time::second_clock::local_time());
      }
      else
      {
         bool succeeded = false;
         for (auto pattern : {"%Y-%m-%d %H:%M:%S", "%B %d, %Y", "%b %d, %Y"})
         {
            if ((succeeded = parse(t, inDateStr, pattern)))
               break;
         }
         if (!succeeded)
            return toValue("Invalid date format!");

         // Fill missing fields
         t = boost::posix_time::to_tm(boost::posix_time::ptime_from_tm(t));
      }

      std::ostringstream oss;
      oss << std::put_time(&t, Expression::value(c, dateFormat).toString().c_str());

      return oss.str();
   }

   virtual void addAttribute(string_view sv) override final
   {
      dateFormat = Expression::toToken(sv);
   }
};

}
}
