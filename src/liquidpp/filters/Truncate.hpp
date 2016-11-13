#pragma once

#include "../Expression.hpp"
#include "Filter.hpp"

namespace liquidpp {
namespace filters {

struct Truncate {
  Value operator()(Value &&val, Value&& maxCountVal, Value&& ellipsisVal) const {
    if (!val.isStringViewRepresentable())
      return std::move(val);

    auto sv = *val;

    auto maxCount = static_cast<size_t>(maxCountVal.integralValue());
    auto u8Len = utf8::characterCount(sv);
    if (u8Len <= maxCount)
      return std::move(val);

    std::string ellips = "...";
    if (ellipsisVal)
      ellips = ellipsisVal.toString();
    auto u8EllipsLen = utf8::characterCount(ellips);
    
    if (u8EllipsLen >= maxCount)
       return ellips;

    return utf8::substr(sv, 0, maxCount - ellips.size()).to_string() + ellips;
  }
};
}
}
