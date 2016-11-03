#pragma once

#include "../config.h"
#include "Block.hpp"

#include <boost/functional/hash.hpp>

namespace liquidpp {

struct Cycle : public Tag {
  std::string keyName;
  SmallVector<Expression::Token, 4> values;

  static std::string generateKeyName(Expression::RawTokens tokens) {
    std::string var;
    var += '\0';
    var += "cyc";

    std::size_t hash = 0;
    assert(!tokens.empty());

    if (tokens.size() == 1 || tokens[1] == ":")
      hash = boost::hash_range(tokens[0].begin(), tokens[0].end());
    else {
      for (auto &&t : tokens) {
        auto h = boost::hash_range(t.begin(), t.end());
        boost::hash_combine(hash, h);
      }
    }

    var.append(reinterpret_cast<const char *>(&hash), sizeof(size_t));
    return var;
  }

  Cycle(Tag &&tag) : Tag(std::move(tag)) {
    auto tokens = Expression::splitTokens(value);
    if (tokens.empty())
      throw Exception("Missing parameters!", value);
    keyName = generateKeyName(tokens);

    size_t startIdx = 0;
    if (tokens.size() >= 2 && tokens[1] == ":")
      startIdx += 2;

    for (size_t i = startIdx; i < tokens.size(); i++) {
      auto &t = tokens[i];
      if (i % 2 == 0)
        values.push_back(Expression::toToken(t));
      else if (t != ",")
        throw Exception("Expected ',' as separator!", t);
    }
  }

  void render(Context &context, std::string &res) const override final {
    auto &dsc = context.documentScopeContext();

    std::intmax_t numVal = 0;
    auto val = dsc.get(keyName);
    if (val.isIntegral())
      numVal = val.integralValue();

    Value v = Expression::value(
        context, values.at(static_cast<size_t>(
                     numVal))); // out_of_range: manipulated template
    res += v.toString();
    numVal++;
    if (static_cast<size_t>(numVal) == values.size())
      numVal = 0;
    dsc.set(keyName, numVal);
  }
};
}