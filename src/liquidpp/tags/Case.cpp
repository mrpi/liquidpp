#include "Case.hpp"

#include "../Context.hpp"

namespace liquidpp {

Case::Case(Tag&& tag)
   : Block(std::move(tag)) {
   auto tokens = Expression::splitTokens(value);
   if (tokens.size() != 1)
      throw Exception("Malformed 'case' tag!", value);

   valueToken = Expression::toToken(tokens[0]);
}

namespace
{
Value caseValue(Context& context, const Node& node)
{
   auto& tag = boost::get<UnevaluatedTag>(node);
   auto tokens = Expression::splitTokens(tag.value);
   if (tokens.size() != 1)
      throw Exception("'case' tag with invalid argument count!", tag.value);
   return Expression::value(context, Expression::toToken(tokens[0]));
}
}

void Case::render(Context& context, std::string& res) const {
   auto actualValue = Expression::value(context, valueToken);

   bool matchingCase = false;
   for (auto&& node : this->body.nodeList)
   {
      if (isSpecificTag(node, "when"))
      {
         auto actStr = actualValue.toString();
         auto caseStr = caseValue(context, node).toString();
         if (actualValue == caseValue(context, node))
            matchingCase = true;
         else if (matchingCase)
            break;
      }
      else if (isSpecificTag(node, "else"))
      {
         if (matchingCase)
            break;
         else
            matchingCase = true;
      }
      else if (matchingCase)
         renderNode(context, node, res);
   }
}

}