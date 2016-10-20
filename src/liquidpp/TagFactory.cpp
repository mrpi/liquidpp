#include "TagFactory.hpp"

#include "tags/Assign.hpp"
#include "tags/Comment.hpp"
#include "tags/For.hpp"
#include "tags/If.hpp"
#include "tags/Capture.hpp"

namespace liquidpp
{
std::shared_ptr<Tag> TagFactory::operator()(UnevaluatedTag&& tag) const
{
   if (tag.name == "for")
      return std::make_shared<For>(std::move(tag));
   if (tag.name == "if")
      return std::make_shared<If>(std::move(tag));
   if (tag.name == "assign")
      return std::make_shared<Assign>(std::move(tag));
   if (tag.name == "comment")
      return std::make_shared<Comment>(std::move(tag));
   if (tag.name == "capture")
      return std::make_shared<Capture>(std::move(tag));

   return nullptr;
}
}
