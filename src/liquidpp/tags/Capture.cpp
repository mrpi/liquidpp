#include "Capture.hpp"

#include "../Context.hpp"

namespace liquidpp
{

Capture::Capture(Tag&& tag)
   : Block(std::move(tag)) {
   auto tokens = Expression::splitTokens(tag.value);
   if (tokens.size() != 1)
      throw Exception("Capture tag requires exactly once argument!", tag.value);

   variableName = tokens[0];
}

void Capture::render(Context& context, std::string& res) const
{
   std::string varOut;
   
   for (auto&& node : body.nodeList)
      renderNode(context, node, varOut);
   
   context.documentScopeContext().set(variableName.to_string(), std::move(varOut));
}

}
