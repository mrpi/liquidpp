#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"
#include "../Context.hpp"

namespace liquidpp
{
namespace filters
{

struct Map : public Filter
{
   Expression::Token subValueToken;

   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isRange())
         return std::move(val);

      RangeDefinition::InlineValues res;

      auto subValue = Expression::value(c, subValueToken).toString();
      auto& range = val.range();
      auto basePath = range.rangePath();
      if (!basePath)
         throw std::runtime_error("Could not map this range (not an range on objects)!");
      auto cnt = range.size();
      res.reserve(cnt);
      for (size_t i=0; i < cnt; i++)
      {
         auto idx = range.index(i);
         res.push_back(c.get(basePath->to_string() + '[' + boost::lexical_cast<std::string>(idx) + "]." + subValue).toString());
      }

      return RangeDefinition{std::move(res)};
   }

   virtual void addAttribute(string_view sv) override final
   {
      if (subValueToken == Expression::Token{})
         subValueToken = Expression::toToken(sv);
      else
         throw Exception("Too many attributes!", sv);
   }
};

}
}