#pragma once

#include "IRenderable.hpp"

namespace liquidpp
{

struct Variable : public IRenderable {
   std::string leftPadding;
   std::string variableName;
   std::string filters;

   bool operator==(const Variable& other) const;

   void render(Context& context, std::string& out) const override final;
};

}
