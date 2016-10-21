#include "For.hpp"

#include "../Context.hpp"

namespace liquidpp {
For::For(Tag&& tag)
   : Block(std::move(tag)) {
   auto tokens = Expression::splitTokens(value);
   if (tokens.size() != 3)
      throw Exception("Malformed 'for' tag!", value);

   loopVariable = tokens[0].to_string();
   if (tokens[1] != "in")
      throw Exception("Second token in 'for' tag has to be the 'in' keyword!", tokens[1]);
   rangeVariable = tokens[2].to_string();
}

void For::render(Context& context, std::string& res) const {
   auto val = context.get(rangeVariable);

   try {
      if (val == ValueTag::Range)
      {
         size_t idx = 0;
         while (true) {
            try {
               std::string idxPath = rangeVariable + '[' + boost::lexical_cast<std::string>(idx++) + ']';
               if (!renderElement(context, res, context.get(idxPath), idxPath))
                  break;
            } catch(DoContinue&) {}
         }
      }
      else if (!val.isNil()) {
         try {
            renderElement(context, res, val, rangeVariable);
         } catch(DoContinue&) {}
      }
   } catch(DoBreak&) {}
}

bool For::renderElement(Context& context, std::string& res, const Value& currentVal, string_view idxPath) const {
   Context loopVarContext{context};

   if (currentVal.isSimpleValue())
      loopVarContext.setLiquidValue(loopVariable, currentVal);
   else if (currentVal != ValueTag::OutOfRange)
      loopVarContext.setLink(loopVariable, idxPath);
   else
      return false;

   for (auto&& node : body.nodeList)
      renderNode(loopVarContext, node, res);

   return true;
}

}