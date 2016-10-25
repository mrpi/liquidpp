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
            limitToken = Expression::toToken(tokens[i+2]);
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
            offsetToken = Expression::toToken(tokens[i+2]);
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

Value For::LoopData::get(OptIndex requIdx, string_view path) const
{
   if (requIdx)
      return ValueTag::Object;

   if (path == "first")
      return toValue(idx == 0);
   if (path == "index")
      return toValue(idx + 1);
   if (path == "index0")
      return toValue(idx);
   if (path == "last")
      return toValue(idx + 1 == size);
   if (path == "length")
      return toValue(size);
   if (path == "rindex")
      return toValue(size - idx);
   if (path == "rindex0")
      return toValue(size - idx - 1);

   return ValueTag::Null;
}

void For::render(Context& context, std::string& res) const {
   auto rangeExpr = toRangeDefinition(rangeVariable);
   size_t rangeExprStart = 0;
   size_t rangeExprEnd = 0;
   Value val;

   if (rangeExpr)
   {
      rangeExprStart = Expression::value(context, rangeExpr->startIdxToken).integralValue();
      rangeExprEnd = Expression::value(context, rangeExpr->endIdxToken).integralValue();
   }
   else
      val = context.get(rangeVariable);

   size_t size = 0;
   if (rangeExpr)
      size = rangeExprEnd - rangeExprStart + 1;
   else if (val.isRange())
      size = val.size();
   else if (val.isSimpleValue())
      size = 1;

   auto getValue = [&](size_t i) mutable {
      if (rangeExpr)
         return std::make_tuple(toValue(rangeExprStart + i), std::string{});
      else if (val.isSimpleValue())
         return std::make_tuple(val, std::string{});
      else if (val.isRange())
         return Expression::value(context, val.range(), i, rangeVariable);

      return std::make_tuple(Value{ValueTag::OutOfRange}, std::string{});
   };

   size_t offset = 0;
   if (offsetToken)
      offset = Expression::value(context, *offsetToken).integralValue();

   size_t limit = size - offset;
   if (limitToken)
   {
      auto l = static_cast<size_t>(Expression::value(context, *limitToken).integralValue());
      if (l < limit)
         limit = l;
   }

   if (limit == 0)
   {
      bool elseFound = false;
      for (auto&& node : body.nodeList)
      {
         if (isSpecificTag(node, "else"))
            elseFound = true;
         else if (elseFound)
            renderNode(context, node, res);
      }
      return;
   }

   for (size_t i = 0; i < limit; i++) {
      size_t idx = i + offset;
      if (reversed)
         idx = size - offset - i - 1;

      auto itm = getValue(idx);
      if (!renderElement(context, res, std::get<0>(itm), std::get<1>(itm), LoopData{i, limit}))
         break;
   }
}

template<>
struct ValueConverter<For::LoopData> : public std::true_type {
   template<typename T>
   static auto get(T&& loopData) {
      return [loopData = std::forward<T>(loopData)](OptIndex
      idx, string_view
      path) -> Value
      {
         return loopData.get(idx, path);
      };
   }
};

bool For::renderElement(Context& context, std::string& res, const Value& currentVal, string_view idxPath, LoopData forLoop) const {
   Context loopVarContext{context};
   loopVarContext.set("forloop", forLoop);

   if (currentVal.isSimpleValue())
      loopVarContext.setLiquidValue(loopVariable, currentVal);
   else if (currentVal != ValueTag::OutOfRange)
      loopVarContext.setLink(loopVariable, idxPath);
   else
      return false;

   try {
      for (auto&& node : body.nodeList)
      {
         if (isSpecificTag(node, "else"))
            break;
         renderNode(loopVarContext, node, res);
      }

      return true;
   }
   catch (DoContinue&) { return true; }
   catch (DoBreak&) { return false; }
}
}
