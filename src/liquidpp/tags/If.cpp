#include "If.hpp"
#include "../ValueConverter.hpp"

#include "../Context.hpp"

namespace liquidpp {

If::If(Tag&& tag)
   : Block(std::move(tag)), expression(Expression::fromSequence(value)) {
}

void If::render(Context& context, std::string& res) const {
   if (expression(context))
   {
      for (auto&& node : body.nodeList)
         renderNode(context, node, res);
   }
}

}
