#pragma once

#include "../Context.hpp"
#include "../Expression.hpp"
#include "Filter.hpp"

namespace liquidpp {
namespace filters {

struct Map {
  Value operator()(Context& c, Value &&val, Value&& subVal) const {
    if (!val.isRange())
      return std::move(val);
    
    Value res = RangeDefinition{RangeDefinition::InlineValues{}};
    auto& out = res.range().inlineValues();

    auto subValue = subVal.toString();
    auto &range = val.range();
    auto basePath = range.rangePath();
    if (basePath.empty())
      throw std::runtime_error(
          "Could not map this range (not an range on objects)!");
    auto cnt = range.size();
    out.reserve(cnt);

    auto subPath = toPath(subValue);

    Path path;
    path.reserve(basePath.size() + 1 + subPath.size());
    path.insert(path.end(), basePath.begin(), basePath.end());
    path.push_back(Key{});
    path.insert(path.end(), subPath.begin(), subPath.end());
    auto &idxKey = path[basePath.size()];

    for (size_t i = 0; i < cnt; i++) {
      idxKey = Key{range.index(i)};
      out.push_back(c.get(path).toString());
    }

    return res;
  }
};
}
}
