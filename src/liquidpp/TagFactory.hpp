#pragma once

#include <memory>

#include "tags/UnevaluatedTag.hpp"
#include "tags/Assign.hpp"

namespace liquidpp
{

struct TagFactoryBase {
   std::shared_ptr<Tag> operator()(UnevaluatedTag&& tag) const;
};

struct TagFactory : public TagFactoryBase {
   template<typename FilterFactoryT>
   std::shared_ptr<Tag> operator()(const FilterFactoryT& filterFac, UnevaluatedTag&& tag) const {
      if (tag.name == "assign")
         return std::make_shared<Assign>(FilterFactoryT{}, std::move(tag));
      return TagFactoryBase::operator()(std::move(tag));
   }
};

}
