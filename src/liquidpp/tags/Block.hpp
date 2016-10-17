#pragma once

#include "Tag.hpp"
#include "../BlockBody.hpp"

namespace liquidpp
{

struct Block : public Tag {
   Block(Tag&& tag)
      : Tag(std::move(tag)) {}

   BlockBody body;
};

}
