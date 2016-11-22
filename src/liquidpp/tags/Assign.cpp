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
         context.documentScopeContext().set(variableName.to_string(), std::vector<std::string>{vals.begin(), vals.end()});
      }
      else
         context.documentScopeContext().setLink(variableName.to_string(), boost::get<Path>(assignment));
   }
   else if (v.isStringView())
      context.documentScopeContext().set(variableName.to_string(), v.toString());
   else
      context.documentScopeContext().setLiquidValue(variableName.to_string(), v);
}

}
