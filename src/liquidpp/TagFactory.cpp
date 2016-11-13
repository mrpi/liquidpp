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

std::unique_ptr<Tag> TagFactoryBase::operator()(UnevaluatedTag&& tag) const
{
   if (tag.name == "for")
      return std::make_unique<For>(std::move(tag));
   if (tag.name == "if")
      return std::make_unique<If>(std::move(tag));
   if (tag.name == "comment")
      return std::make_unique<Comment>(std::move(tag));
   if (tag.name == "capture")
      return std::make_unique<Capture>(std::move(tag));
   if (tag.name == "break")
      return std::make_unique<Break>(std::move(tag));
   if (tag.name == "continue")
      return std::make_unique<Continue>(std::move(tag));
   if (tag.name == "case")
      return std::make_unique<Case>(std::move(tag));
   if (tag.name == "unless")
      return std::make_unique<Unless>(std::move(tag));
   if (tag.name == "increment")
      return std::make_unique<Increment>(std::move(tag));
   if (tag.name == "decrement")
      return std::make_unique<Decrement>(std::move(tag));
   if (tag.name == "cycle")
      return std::make_unique<Cycle>(std::move(tag));

   return nullptr;
}
}
