#include "For.hpp"

#include "../Context.hpp"

namespace liquidpp {
For::For(Tag&& tag)
   : Block(std::move(tag)) {
   auto tokens = Expression::splitTokens(value);

   loopVariable = tokens[0].to_string();
   if (tokens[1] != "in")
      throw Exception("Second token in 'for' tag has to be the 'in' keyword!", tokens[1]);
   rangeVariable = tokens[2].to_string();

   auto cnt = tokens.size();
   for (size_t i=3; i < cnt; i++)
   {
      auto& key = tokens[i];

      if (key == "limit")
      {
         if (i+2 < cnt)
         {
            if (tokens[i+1] != ":")
               throw Exception("Expected operator ':' after parameter name!", tokens[i+1]);
            limit = Expression::toToken(tokens[i+2]);
            i += 2;
         }
         else
            throw Exception("Not enough arguments for this parameter!", key);
      }
      else if (key == "offset")
      {
         if (i+2 < cnt)
         {
            if (tokens[i+1] != ":")
               throw Exception("Expected operator ':' after parameter name!", tokens[i+1]);
            offset = Expression::toToken(tokens[i+2]);
            i += 2;
         }
         else
            throw Exception("Not enough arguments for this parameter!", key);
      }
      else if (key == "reversed")
         reversed = true;
      else
         throw Exception("Unknown parameter on 'for' tag!", key);
   }
}

boost::optional<For::RangeExpression> For::toRangeDefinition(string_view sv)
{
   if (sv.front() == '(')
   {
      if (sv.back() != ')')
         throw Exception("Unterminated range expression!", sv);

      sv.remove_prefix(1);
      sv.remove_suffix(1);

      RangeExpression res;
      auto idx = sv.find("..");
      if (idx == std::string::npos)
         throw Exception("Range expression misses '..'!", sv);
      res.startIdxToken = Expression::toToken(sv.substr(0, idx));
      res.endIdxToken = Expression::toToken(sv.substr(idx+2));

      return res;
   }

   return boost::none;
}

void For::render(Context& context, std::string& res) const {
   auto rangeExpr = toRangeDefinition(rangeVariable);
   if (rangeExpr)
   {
      renderOnRange(context, res, *rangeExpr);
      return;
   }

   auto val = context.get(rangeVariable);

   size_t itmIdx = 0;
   boost::optional<size_t> limitSize;
   if (limit)
      limitSize = Expression::value(context, *limit).integralValue();

   try {
      if (val == ValueTag::Range)
      {
         size_t idx = 0;
         while (true) {
            if (limitSize && itmIdx++ == *limitSize)
               break;

            try {
               std::string idxPath = rangeVariable + '[' + boost::lexical_cast<std::string>(idx++) + ']';
               if (!renderElement(context, res, context.get(idxPath), idxPath))
                  break;
            } catch(DoContinue&) {}
         }
      }
      else if (!val.isNil()) {
         try {
            if (limitSize && itmIdx++ == 0)
               return;

            renderElement(context, res, val, rangeVariable);
         } catch(DoContinue&) {}
      }
   } catch(DoBreak&) {}
}

void For::renderOnRange(Context& context, std::string& res, const RangeExpression& range) const
{
   auto start = Expression::value(context, range.startIdxToken).integralValue();
   auto end = Expression::value(context, range.endIdxToken).integralValue();

   size_t itmIdx = 0;
   boost::optional<size_t> limitSize;
   if (limit)
      limitSize = Expression::value(context, *limit).integralValue();

   try {
      for (auto i = start; i <= end; ++i) {
         if (limitSize && itmIdx++ == *limitSize)
            break;

         try {
            renderElement(context, res, toValue(i), string_view{});
         } catch (DoContinue&) {}
      }
   } catch (DoBreak&) {}
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