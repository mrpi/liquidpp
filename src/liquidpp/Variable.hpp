#pragma once

#include "IRenderable.hpp"
#include "Expression.hpp"
#include "filters/Filter.hpp"
#include "Exception.hpp"

namespace liquidpp
{

struct Variable {
   Expression::Token variable;
   boost::optional<Expression::FilterChain> filterChain;

   Variable() = default;
   //~Variable() final = default;

   Variable(Expression::Token&& var)
    : variable(std::move(var))
   {
   }

   Variable(Expression::Token&& var, Expression::FilterChain&& fc)
    : variable(std::move(var)), filterChain(std::move(fc))
   {
   }

   bool operator==(const Variable& other) const;

   void render(Context& context, std::string& out) const;
};

}
