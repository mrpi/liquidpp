#pragma once

#include "Tag.hpp"

namespace liquidpp
{

struct UnevaluatedTag : public Tag {
   void render(Context& context, std::string& out) const override final {
   }
};

};
