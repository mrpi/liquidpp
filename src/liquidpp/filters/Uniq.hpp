#pragma once

#include <set>

#include "Filter.hpp"
#include "../Expression.hpp"

namespace liquidpp
{
namespace filters
{

struct Uniq
{
   Value operator()(Value&& val) const
   {
      if (!val.isRange())
         return std::move(val);

      auto& vals = val.range().inlineValues();
      std::set<string_view> foundValues;
      auto itr = std::remove_if(vals.begin(), vals.end(), [&](const auto& val){
         return !foundValues.insert(val).second;
      });
      vals.erase(itr, vals.end());

      return std::move(val);
   }
};

}
}