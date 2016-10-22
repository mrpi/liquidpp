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
   boost::optional<Expression::Token> limitToken;
   boost::optional<Expression::Token> offsetToken;
   bool reversed{false};

   For(Tag&& tag);

   struct RangeExpression
   {
      Expression::Token startIdxToken;
      Expression::Token endIdxToken;
   };

   struct LoopData
   {
      size_t idx{0};
      size_t size{0};

      LoopData() = default;

      LoopData(size_t idx, size_t size)
        : idx(idx), size(size)
      {}

      Value get(OptIndex idx, string_view path) const;
   };

   void render(Context& context, std::string& res) const override final;

private:
   bool renderElement(Context& context, std::string& res, const Value& currentVal, string_view idxPath, LoopData forLoop) const;
   static boost::optional<RangeExpression> toRangeDefinition(string_view sv);
};

}