#include "TagFactory.hpp"

#include "tags/Assign.hpp"
#include "tags/Comment.hpp"
#include "tags/For.hpp"
#include "tags/Conditional.hpp"
#include "tags/Capture.hpp"
#include "tags/Case.hpp"
#include "tags/Increment.hpp"
#include "tags/Cycle.hpp"

namespace liquidpp
{

std::shared_ptr<Tag> TagFactoryBase::operator()(UnevaluatedTag&& tag) const
{
   if (tag.name == "for")
      return std::make_shared<For>(std::move(tag));
   if (tag.name == "if")
      return std::make_shared<If>(std::move(tag));
   if (tag.name == "comment")
      return std::make_shared<Comment>(std::move(tag));
   if (tag.name == "capture")
      return std::make_shared<Capture>(std::move(tag));
   if (tag.name == "break")
      return std::make_shared<Break>(std::move(tag));
   if (tag.name == "continue")
      return std::make_shared<Continue>(std::move(tag));
   if (tag.name == "case")
      return std::make_shared<Case>(std::move(tag));
   if (tag.name == "unless")
      return std::make_shared<Unless>(std::move(tag));
   if (tag.name == "increment")
      return std::make_shared<Increment>(std::move(tag));
   if (tag.name == "decrement")
      return std::make_shared<Decrement>(std::move(tag));
   if (tag.name == "cycle")
      return std::make_shared<Cycle>(std::move(tag));

   return nullptr;
}
}
