#pragma once

#include "IRenderable.hpp"
#include "Expression.hpp"
#include "filters/Filter.hpp"
#include "Exception.hpp"

namespace liquidpp
{

struct Variable : public IRenderable {
   Expression::Token variable;
   SmallVector<std::shared_ptr<filters::Filter>, 2> filterChain;

   Variable() = default;

   template<typename FilterFactoryT>
   Variable(const FilterFactoryT& filterFac, string_view data)
   {
      data.remove_prefix(data[2] == '-' ? 3 : 2);
      if (data.size() < 3)
         throw Exception("Tag is too short!", data);
      data.remove_suffix(data[data.size() - 3] == '-' ? 3 : 2);

      auto tokens = Expression::splitTokens(data);
      if (tokens.empty())
         throw Exception("Variable definition without token!", data);
      variable = Expression::toToken(tokens[0]);

      filterChain = Expression::toFilterChain(filterFac, tokens, 1);
   }

   bool operator==(const Variable& other) const;

   void render(Context& context, std::string& out) const override final;
};

}
