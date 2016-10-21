#pragma once

#include "Block.hpp"
#include "../config.h"

namespace liquidpp
{

class Value;

struct DoBreak : public std::logic_error
{
   DoBreak()
      : std::logic_error("'break' tag outside of for-loop!")
   {}
};

struct DoContinue : public std::logic_error
{
   DoContinue()
      : std::logic_error("'continue' tag outside of for-loop!")
   {}
};

struct Break : public Tag
{
   Break(Tag&& tag)
      : Tag(std::move(tag))
   {}

   void render(Context& context, std::string& out) const override final
   {
      throw DoBreak{};
   }
};

struct Continue : public Tag
{
   Continue(Tag&& tag)
      : Tag(std::move(tag))
   {}

   void render(Context& context, std::string& out) const override final
   {
      throw DoContinue{};
   }
};

struct For : public Block
{
   std::string loopVariable;
   std::string rangeVariable;
   boost::optional<Expression::Token> limit;
   boost::optional<Expression::Token> offset;
   bool reversed{false};

   For(Tag&& tag);

   struct RangeExpression
   {
      Expression::Token startIdxToken;
      Expression::Token endIdxToken;
   };

   void render(Context& context, std::string& res) const override final;

private:
   void renderOnRange(Context& context, std::string& res, const RangeExpression& range) const;
   bool renderElement(Context& context, std::string& res, const Value& currentVal, string_view idxPath) const;
   static boost::optional<RangeExpression> toRangeDefinition(string_view sv);
};

}