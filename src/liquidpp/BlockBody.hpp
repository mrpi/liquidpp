#pragma once

#include "tags/UnevaluatedTag.hpp"
#include "Variable.hpp"

namespace liquidpp
{

using Node = boost::variant<string_view, Variable, UnevaluatedTag, std::shared_ptr<const IRenderable>>;

enum class NodeType {
   String = 0,
   Variable = 1,
   UnevaluatedTag = 2,
   Tag = 3
};

void renderNode(Context& context, const Node& node, std::string& res);

inline NodeType type(const Node& n) {
   return static_cast<NodeType>(n.which());
}

struct BlockBody {
   std::vector<Node> nodeList;
};

}
