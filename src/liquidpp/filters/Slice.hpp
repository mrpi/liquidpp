#pragma once

#include "Filter.hpp"

namespace liquidpp {
namespace filters {

struct Slice : public Filter {
  Expression::Token startIdxToken;
  Expression::Token endIdxToken;

  virtual Value operator()(Context &c, Value &&val) const override final {
    auto startIdx = Expression::value(c, startIdxToken).integralValue();

    auto str = val.toString();

    if (endIdxToken == Expression::Token{}) {
      if (startIdx < 0)
        return str.substr(static_cast<size_t>(str.size() + startIdx), 1);
      return str.substr(static_cast<size_t>(startIdx), 1);
    } else {
      auto endIdx = static_cast<size_t>(
          Expression::value(c, endIdxToken).integralValue());
      if (startIdx < 0)
        return str.substr(static_cast<size_t>(str.size() + startIdx), endIdx);
      return str.substr(static_cast<size_t>(startIdx), endIdx);
    }
  }

  virtual void addAttribute(string_view sv) override final {
    if (startIdxToken == Expression::Token{})
      startIdxToken = Expression::toToken(sv);
    else if (endIdxToken == Expression::Token{})
      endIdxToken = Expression::toToken(sv);
    else
      throw Exception("Too many attributes!", sv);
  }
};
}
}