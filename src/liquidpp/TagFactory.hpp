#pragma once

#include <memory>

#include "tags/UnevaluatedTag.hpp"

namespace liquidpp
{

struct TagFactory {
   std::shared_ptr<Tag> operator()(UnevaluatedTag&& tag) const;
};

}
