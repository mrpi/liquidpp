#include "Template.hpp"

#include "Context.hpp"

namespace liquidpp
{

std::ostream& operator<<(std::ostream& os, NodeType t) {
   switch (t) {
      case NodeType::String:
         return os << "String";
      case NodeType::Variable:
         return os << "Variable";
      case NodeType::UnevaluatedTag:
         return os << "UnevaluatedTag";
      case NodeType::Tag:
         return os << "Tag";
   }

   assert(false);
   return os << "<unknown>";
}

std::string Template::operator()(const Context& context) const {
   std::string res;
   Context mutableScopedContext{&context};

   for (auto&& node : root.nodeList)
      renderNode(mutableScopedContext, node, res);

   return res;
}

}