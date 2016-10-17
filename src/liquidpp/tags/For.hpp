#pragma once

#include "Block.hpp"
#include "../config.h"

namespace liquidpp
{

class Value;

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