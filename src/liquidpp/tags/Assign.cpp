#include "Assign.hpp"

namespace liquidpp
{

void Assign::render(Context& context, std::string& res) const
{
   auto v = Expression::value(context, assignment, filterChain);
   if (v == ValueTag::Object || v.isRange())
      context.documentScopeContext().setLink(variableName.to_string(), boost::get<Expression::VariableName>(assignment).name);
   else
      context.documentScopeContext().setLiquidValue(variableName.to_string(), v);
}

}