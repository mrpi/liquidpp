#include "BlockBody.hpp"

#include <boost/variant/get.hpp>

namespace liquidpp
{
void renderNode(Context& context, const Node& node, std::string& res)
{
   // TODO: use static visitor
   switch(type(node))
   {
      case NodeType::String:
      {
         auto sv = boost::get<string_view>(node);
         res.append(sv.data(), sv.size());
         break;
      }
      case NodeType::Variable:
      {
         boost::get<Variable>(node).render(context, res);
         break;
      }
      case NodeType::Tag:
      {
         auto& renderable = *boost::get<std::unique_ptr<const IRenderable>>(node);
         renderable.render(context, res);
         break;
      }
      case NodeType::UnevaluatedTag:
         break;
   }
}

}
