#pragma once

#include "../Expression.hpp"
#include "Filter.hpp"

#include <boost/date_time/posix_time/conversion.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

#include <time.h>

namespace liquidpp {

using namespace std::literals;

namespace filters {
   
struct Date {
  static bool parse(boost::posix_time::ptime &t, const std::string &str,
                    const std::locale& loc) {
    std::istringstream iss(str);
    iss.imbue(loc);
    iss >> t;
    if (iss.fail())
      return false;
    return true;
  }

  Value operator()(Value &&val, Value&& dateFormat) const {
    if (!val.isStringViewRepresentable())
      return std::move(val);

    std::tm t = {};

    auto inDateStr = val.toString();
    if (inDateStr == "now" || inDateStr == "today") {
      t = boost::posix_time::to_tm(
          boost::posix_time::second_clock::local_time());
    } else {
      bool succeeded = false;
      
      static std::vector<std::locale> locs = {
         {std::locale(std::locale::classic(), new boost::posix_time::time_input_facet("%Y-%m-%d %H:%M:%S"s))},
         {std::locale(std::locale::classic(), new boost::posix_time::time_input_facet("%B %d, %Y"s))},
         {std::locale(std::locale::classic(), new boost::posix_time::time_input_facet("%b %d, %Y"s))}
      };
      
      for (auto loc : locs) {
        boost::posix_time::ptime pt;
        if ((succeeded = parse(pt, inDateStr, loc))) {
          // Fill missing fields
          t = boost::posix_time::to_tm(pt);

          break;
        }
      }
      if (!succeeded)
        return toValue("Invalid date format!");
    }

    std::ostringstream oss;
    oss << std::put_time(&t, dateFormat.toString().c_str());

    return oss.str();
  }
};

#ifdef LIQUIDPP_OLD_DATE_IMPL
struct DateOldImpl {
  static bool parse(boost::posix_time::ptime &t, const std::string &str,
                    const char *pattern) {
    std::istringstream iss(str);
    std::locale loc(std::locale::classic(),
                    new boost::posix_time::time_input_facet(pattern));
    iss.imbue(loc);
    iss >> t;
    if (iss.fail())
      return false;
    return true;
  }

  Value operator()(Value &&val, Value&& dateFormat) const {
    if (!val.isStringViewRepresentable())
      return std::move(val);

    std::tm t = {};

    auto inDateStr = val.toString();
    if (inDateStr == "now" || inDateStr == "today") {
      t = boost::posix_time::to_tm(
          boost::posix_time::second_clock::local_time());
    } else {
      bool succeeded = false;
      for (auto pattern : {"%Y-%m-%d %H:%M:%S", "%B %d, %Y", "%b %d, %Y"}) {
        boost::posix_time::ptime pt;
        if ((succeeded = parse(pt, inDateStr, pattern))) {
          // Fill missing fields
          t = boost::posix_time::to_tm(pt);

          break;
        }
      }
      if (!succeeded)
        return toValue("Invalid date format!");
    }

    std::ostringstream oss;
    oss << std::put_time(&t, dateFormat.toString().c_str());

    return oss.str();
  }
};
#endif
   
}
}
