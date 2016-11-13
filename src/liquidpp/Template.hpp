#pragma once

#include "config.h"
#include "BlockBody.hpp"

namespace liquidpp {

std::ostream& operator<<(std::ostream& os, NodeType t);

void renderNode(Context& context, const Node& node, std::string& res);

struct Template {
   BlockBody root;
   mutable size_t mMaxResultSize{0};

   std::string operator()(const Context& context) const;
      
   Exception::Position findPosition(string_view needle) const;
};

}
