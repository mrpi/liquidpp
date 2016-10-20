#pragma once

#include "Block.hpp"
#include "../config.h"

namespace liquidpp
{

struct Capture : public Block
{
   string_view variableName;

   Capture(Tag&& tag);

   void render(Context& context, std::string& res) const override final;
};

}