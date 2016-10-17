#pragma once

#include "../IRenderable.hpp"

namespace liquidpp
{
struct Tag : public IRenderable {
   std::string leftPadding;
   std::string name;
   std::string value;

   auto tie() const {
      return std::tie(leftPadding, name, value);
   }

   bool operator==(const Tag& other) const {
      return tie() == other.tie();
   }

   friend std::ostream& operator<<(std::ostream& os, const Tag& tag) {
      return os << "{%" << tag.leftPadding << tag.name << tag.value << "%}";
   }
};
}
