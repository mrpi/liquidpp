#pragma once

#include "../Expression.hpp"
#include "Filter.hpp"

namespace liquidpp {
namespace filters {

struct Truncate : public Filter {
  Expression::Token maxCountToken;
  Expression::Token ellipsisToken;

  virtual Value operator()(Context &c, Value &&val) const override final {
    if (!val.isStringViewRepresentable())
      return std::move(val);

    auto sv = *val;

    auto maxCount = static_cast<size_t>(
        Expression::value(c, maxCountToken).integralValue());
    auto u8Len = utf8::characterCount(sv);
    if (u8Len <= maxCount)
      return std::move(val);

    std::string ellips = "...";
    if (!(ellipsisToken == Expression::Token{}))
      ellips = Expression::value(c, ellipsisToken).toString();
    auto u8EllipsLen = utf8::characterCount(ellips);
    
    if (u8EllipsLen >= maxCount)
       return ellips;

    return utf8::substr(sv, 0, maxCount - ellips.size()).to_string() + ellips;
  }

  virtual void addAttribute(string_view sv) override final {
    if (maxCountToken == Expression::Token{})
      maxCountToken = Expression::toToken(sv);
    else if (ellipsisToken == Expression::Token{})
      ellipsisToken = Expression::toToken(sv);
    else
      throw Exception("Too many attributes!", sv);
  }
};
}
}
