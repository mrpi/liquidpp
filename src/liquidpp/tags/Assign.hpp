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
   Expression::FilterChain filterChain;

   template<typename FilterFactoryT>
   Assign(const FilterFactoryT& filterFac, Tag&& tag)
    : Tag(std::move(tag))
   {
      auto tokens = Expression::splitTokens(value);
      if (tokens.size() < 3)
         throw Exception("Malformed assign statement (three tokens required)!", value);
      if (tokens[1] != "=")
         throw Exception("Malformed assign statement (assignment operator '=' required)!", value);

      variableName = tokens[0];
      Expression::assureIsSingleKeyPath(variableName);
      assignment = Expression::toToken(tokens[2]);
      filterChain = Expression::toFilterChain(filterFac, tokens, 3);
   }

   void render(Context& context, std::string& res) const override final;
};
}
