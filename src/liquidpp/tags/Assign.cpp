#include "Assign.hpp"

namespace liquidpp
{

void Assign::render(Context& context, std::string& res) const
{
   auto v = Expression::value(context, assignment, filterChain);
   if (v == ValueTag::Object || v.isRange())
   {
      if (v.isRange() && v.range().usesInlineValues())
      {
         auto& vals = v.range().inlineValues();
         context.documentScopeContext().set(to_string(variableName), std::vector<std::string>{vals.begin(), vals.end()});
      }
      else
         context.documentScopeContext().setLink(to_string(variableName), boost::get<Path>(assignment));
   }
   else if (v.isStringView())
      context.documentScopeContext().set(to_string(variableName), v.toString());
   else
      context.documentScopeContext().setLiquidValue(to_string(variableName), v);
}

}
