#pragma once

#include "Block.hpp"
#include "../config.h"
#include "../Expression.hpp"
#include "../Context.hpp"

#include <boost/optional.hpp>

namespace liquidpp {

struct Assign : public Tag {
   string_view variableName;
   Expression::Token assignment;

   Assign(Tag&& tag);

   void render(Context& context, std::string& res) const override final;
};
}