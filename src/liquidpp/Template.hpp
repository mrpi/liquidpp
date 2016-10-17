#pragma once

#include "config.h"
#include "BlockBody.hpp"

namespace liquidpp {

std::ostream& operator<<(std::ostream& os, NodeType t);

void renderNode(Context& context, const Node& node, std::string& res);

struct Template {
   BlockBody root;

   std::string operator()(const Context& context) const;
};

}
