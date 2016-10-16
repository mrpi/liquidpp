#pragma once

#include "config.h"
#include "template.hpp"

namespace liquidpp {
namespace impl {
BlockBody parseFlat(string_view content);

template<typename TagFactoryT, typename Iterator>
BlockBody buildBlocks(Iterator& itr, const Iterator& end, const std::string& endTagName = "") {
   BlockBody res;
   res.nodeList.reserve(end - itr);
   TagFactory tagFac;

   for (; itr != end; ++itr) {
      auto& node = *itr;

      if (type(node) == NodeType::UnevaluatedTag) {
         auto& rawTag = boost::get<UnevaluatedTag>(node);
         if (rawTag.name == endTagName)
            break;

         auto tag = TagFactoryT{}(std::move(rawTag));
         if (tag) {
            auto block = dynamic_cast<Block*>(tag.get());
            if (block)
               // TODO: get rid of recursion
               block->body = buildBlocks<TagFactoryT>(++itr, end, "end" + tag->name);

            node = std::move(tag);
         }
      }

      res.nodeList.push_back(std::move(node));
   }

   if (itr == end && !endTagName.empty())
      throw std::runtime_error("Missing closing tag '{%" + endTagName + "%}'");

   return res;
}

}

template<typename TagFactoryT = TagFactory>
Template parse(string_view content) {
   liquidpp::Template ast;

   BlockBody flatNodes = impl::parseFlat(content);

   auto itr = flatNodes.nodeList.begin();
   ast.root = impl::buildBlocks<TagFactoryT>(itr, flatNodes.nodeList.end());

   return ast;
}
}

