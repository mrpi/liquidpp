#pragma once

#include "../IRenderable.hpp"

namespace liquidpp
{
struct Tag : public IRenderable {
   std::string leftPadding;
   std::string name;
   std::string value;

   bool operator==(const Tag& other) const;
};
}
