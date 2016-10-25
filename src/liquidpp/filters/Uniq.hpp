#pragma once

#include <set>

#include "Filter.hpp"
#include "../Expression.hpp"

namespace liquidpp
{
namespace filters
{

struct Uniq : public Filter
{
   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isRange())
         return std::move(val);

      auto& vals = val.range().inlineValues();
      std::set<std::string> foundValues;
      auto itr = std::remove_if(vals.begin(), vals.end(), [&](const std::string& val){
         if (foundValues.find(val) == foundValues.end())
         {
            foundValues.insert(val);
            return false;
         }
         return true;
      });
      vals.erase(itr, vals.end());

      return std::move(val);
   }
};

}
}