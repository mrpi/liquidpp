#pragma once

#include "../IRenderable.hpp"
#include "../config.h"

namespace liquidpp
{
struct Tag : public IRenderable {
   string_view name;
   string_view value;

   bool operator==(const Tag& other) const;
};
}
