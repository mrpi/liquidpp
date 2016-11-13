#pragma once

#include "../Expression.hpp"
#include "Filter.hpp"

namespace liquidpp {
namespace filters {

struct TruncateWords {
  Value operator()(Value &&val, Value&& maxWordCountVal, Value&& ellipsisVal) const {
    if (!val.isStringViewRepresentable())
      return std::move(val);

    auto sv = *val;

    std::string res;
    res.reserve(sv.size());

    size_t wordCount = 0;
    auto maxWordCount = static_cast<size_t>(maxWordCountVal.integralValue());
    std::string ellips = "...";
    if (ellipsisVal)
      ellips = ellipsisVal.toString();

    bool lastWasWhitespace = true;
    for (auto c : sv) {
      if (Expression::isWhitespace(c)) {
        if (wordCount == maxWordCount)
          break;
        lastWasWhitespace = true;
      } else {
        if (lastWasWhitespace) {
          wordCount++;
          lastWasWhitespace = false;
        }
      }
      res += c;
    }

    if (res.size() < sv.size())
      res += ellips;

    return std::move(res);
  }
};
}
}
