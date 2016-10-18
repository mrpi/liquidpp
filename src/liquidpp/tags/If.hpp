#pragma once

#include "Block.hpp"
#include "../config.h"
#include "../Expression.hpp"

#include <boost/optional.hpp>

namespace liquidpp {

struct If : public Block {
   Expression expression;

   If(Tag&& tag);

   void render(Context& context, std::string& res) const override final;

};
}