#pragma once

#include "config.h"
#include "Template.hpp"
#include "tags/Block.hpp"
#include "tags/UnevaluatedTag.hpp"
#include "TagFactory.hpp"
#include "FilterFactory.hpp"

#include <boost/variant/get.hpp>

namespace liquidpp {

namespace impl {
BlockBody parseFlat(string_view content);

namespace
{
inline string_view popString(string_view& str) {
   auto len = str.size();
   for (size_t i = 0; i < len; i++) {
      auto c = str[i];
      if (c == '{' || c == '%') {
         if (i != 0 && str[i-1] == '{') {
            auto res = str.substr(0, i-1);
            str.remove_prefix(i-1);

            if (i+1 < len && str[2] == '-')
            {
               auto idx = res.find_last_not_of(" \t\r\n");
               if (idx == std::string::npos)
                  return string_view{};
               else
                  res.remove_suffix(res.size() - idx - 1);
            }

            return res;
         }
      }
   }

   auto res = str;
   str.remove_suffix(len);
   return res;
}

inline string_view popTag(string_view& str, bool& stripNextString) {
   assert(str.substr(0, 2) == "{%");

   auto len = str.size();
   for (size_t i = 0; i < len; i++) {
      if (str[i] == '}') {
         if (str[i-1] == '%') {
            stripNextString = str[i-2] == '-';
            auto res = str.substr(0, i+1);
            str.remove_prefix(i+1);
            return res;
         }
      }
   }

   throw Exception("Unterminated tag!", str.substr(0, 2));
}

inline string_view popVariable(string_view& str, bool& stripNextString) {
   assert(str.substr(0, 2) == "{{");

   auto len = str.size();
   for (size_t i = 0; i < len; i++) {
      if (str[i] == '}') {
         if (str[i-1] == '}') {
            stripNextString = str[i-2] == '-';
            auto res = str.substr(0, i+1);
            str.remove_prefix(i+1);
            return res;
         }
      }
   }

   throw Exception("Unterminated tag!", str.substr(0, 2));
}

template<typename FilterFactoryT>
void fastParser(string_view content, BlockBody& flatNodes)
{
   auto& nodes = flatNodes.nodeList;

   enum class State
   {
      String,
      Variable,
      Tag
   };

   auto getType = [](string_view cont){
      auto prefix = cont.substr(0, 2);
      if (prefix == "{{")
         return State::Variable;
      if (prefix == "{%")
         return State::Tag;
      return State::String;
   };

   bool stripLeadingWhitespace = false;
   while(!content.empty())
   {
      if (stripLeadingWhitespace)
      {
         auto pos = content.find_first_not_of(" \t\r\n");
         if (pos != std::string::npos)
            content.remove_prefix(pos);
         stripLeadingWhitespace = false;
      }

      switch(getType(content))
      {
         case State::String:
            flatNodes.nodeList.emplace_back(popString(content));
            break;
         case State::Tag:
         {
            auto tagStr = popTag(content, stripLeadingWhitespace);
            nodes.emplace_back(UnevaluatedTag{tagStr});
            break;
         }
         case State::Variable:
         {
            auto varStr = popVariable(content, stripLeadingWhitespace);
            nodes.emplace_back(Variable{FilterFactoryT{}, varStr});
            break;
         }
      }
   }
}
}

template<typename TagFactoryT, typename FilterFactoryT, typename Iterator>
BlockBody buildBlocks(Iterator& itr, const Iterator& end, const std::string& endTagName = "") {
   BlockBody res;
   res.nodeList.reserve(end - itr);

   for (; itr != end; ++itr) {
      auto& node = *itr;

      auto nodeType = type(node);
      if (nodeType == NodeType::UnevaluatedTag) {
         auto& rawTag = boost::get<UnevaluatedTag>(node);
         if (rawTag.name == endTagName)
            break;

         auto tag = TagFactoryT{}(std::move(rawTag));
         if (tag) {
            auto block = dynamic_cast<Block*>(tag.get());
            if (block)
               // TODO: get rid of recursion
               block->body = buildBlocks<TagFactoryT, FilterFactoryT>(++itr, end, "end" + tag->name.to_string());

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

template<typename TagFactoryT = TagFactory, typename FilterFactoryT = FilterFactory>
Template parse(string_view content) {
   liquidpp::Template ast;

   BlockBody flatNodes;
   impl::fastParser<FilterFactoryT>(content, flatNodes);

   auto itr = flatNodes.nodeList.begin();
   ast.root = impl::buildBlocks<TagFactoryT, FilterFactoryT>(itr, flatNodes.nodeList.end());

   return ast;
}
}

