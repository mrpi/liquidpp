#pragma once

#include "Block.hpp"
#include "../config.h"

namespace liquidpp
{

struct Case : public Block
{
   Expression::Token valueToken;

   Case(Tag&& tag);

   void render(Context& context, std::string& res) const override final;
};

}