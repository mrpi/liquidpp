#pragma once

#include "Filter.hpp"

namespace liquidpp {
namespace filters {

struct Slice {
  Value operator()(Value &&val, Value&& startIdxVal, Value&& endIdxVal) const {
    auto startIdx = startIdxVal.integralValue();
    auto str = val.toString();

    if (!endIdxVal) {
      if (startIdx < 0)
        return str.substr(static_cast<size_t>(str.size() + startIdx), 1);
      return str.substr(static_cast<size_t>(startIdx), 1);
    } else {
      auto endIdx = static_cast<size_t>(endIdxVal.integralValue());
      if (startIdx < 0)
        return str.substr(static_cast<size_t>(str.size() + startIdx), endIdx);
      return str.substr(static_cast<size_t>(startIdx), endIdx);
    }
  }
};
}
}
