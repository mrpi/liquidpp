#include "Assign.hpp"

namespace liquidpp
{
Assign::Assign(Tag&& tag)
 : Tag(std::move(tag))
{
   auto tokens = Expression::splitTokens(value);
   if (tokens.size() != 3)
      throw std::runtime_error("Malformed assign statement (three tokens required)!");
   if (tokens[1] != "=")
      throw std::runtime_error("Malformed assign statement (assignment operator '=' required)!");

   variableName = tokens[0];
   assignment = Expression::toToken(tokens[2]);
}

void Assign::render(Context& context, std::string& res) const
{
   auto v = Expression::value(context, assignment);
   if (v == ValueTag::Object || v == ValueTag::Range)
      context.documentScopeContext().setLink(variableName.to_string(), boost::get<Expression::VariableName>(assignment).name);
   else
      context.documentScopeContext().setLiquidValue(variableName.to_string(), v);
}

}