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

   For(Tag&& tag);

   void render(Context& context, std::string& res) const override final;

private:
   bool renderElement(Context& context, std::string& res, const Value& currentVal, string_view idxPath) const;
};

}