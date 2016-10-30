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

inline bool isSpecificTag(const Tag& tag, string_view name)
{
   if (tag.name == name)
      return true;

   return false;
}

inline bool isSpecificTag(const Node& node, string_view name)
{
   if (type(node) == NodeType::UnevaluatedTag)
      return isSpecificTag(boost::get<UnevaluatedTag>(node), name);

   return false;
}

struct BlockBody {
   using Nodes = SmallVector<Node, 4>;

   Nodes nodeList;
   string_view templateRange;
};

}
