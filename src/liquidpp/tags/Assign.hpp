#pragma once

#include "Block.hpp"
#include "../config.h"
#include "../Expression.hpp"
#include "../Context.hpp"

#include <boost/optional.hpp>

namespace liquidpp {

struct Assign : public Tag {
   string_view variableName;
   Expression::Token assignment;
   SmallVector<std::shared_ptr<filters::Filter>, 2> filterChain;

   template<typename FilterFactoryT>
   Assign(const FilterFactoryT& filterFac, Tag&& tag)
    : Tag(std::move(tag))
   {
      auto tokens = Expression::splitTokens(value);
      if (tokens.size() < 3)
         throw std::runtime_error("Malformed assign statement (three tokens required)!");
      if (tokens[1] != "=")
         throw std::runtime_error("Malformed assign statement (assignment operator '=' required)!");

      variableName = tokens[0];
      assignment = Expression::toToken(tokens[2]);
      filterChain = Expression::toFilterChain(filterFac, tokens, 3);
   }

   void render(Context& context, std::string& res) const override final;
};
}