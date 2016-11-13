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
   const auto len = str.size();
   for (size_t i = 0; i < len; i++) {
      const auto c = str[i];
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
   for (size_t i = 3; i < len; i++) {
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
   for (size_t i = 3; i < len; i++) {
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

struct BlockItem
{
   BlockBody* body;
   boost::optional<std::string> endTagName;
};

template<typename TagFactoryT, typename FilterFactoryT>
void fastParser(string_view content, BlockBody& rootBlock)
{
   SmallVector<BlockItem, 4> stack{{&rootBlock}};
   auto block = &stack.back();

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
         else
            return;
         stripLeadingWhitespace = false;
      }

      switch(getType(content))
      {
         case State::String:
            block->body->nodeList.emplace_back(popString(content));
            break;
         case State::Tag:
         {
            UnevaluatedTag rawTag{popTag(content, stripLeadingWhitespace)};
            if (block->endTagName && rawTag.name == *block->endTagName)
            {
               stack.resize(stack.size()-1);
               block = &stack.back();
            }
            else
            {
               auto tag = TagFactoryT{}(FilterFactoryT{}, std::move(rawTag));
               if (tag)
               {
                  auto subBlock = dynamic_cast<Block*>(tag.get());
                  block->body->nodeList.push_back(std::move(tag));
                  if (subBlock)
                  {
                     std::string endTagName = "end";
                     endTagName.append(subBlock->name.data(), subBlock->name.size());
                     stack.push_back({&subBlock->body, std::move(endTagName)});
                     block = &stack.back();
                  }
               }
               else
                  block->body->nodeList.push_back(std::move(rawTag));
            }
            break;
         }
         case State::Variable:
         {
            auto varStr = popVariable(content, stripLeadingWhitespace);
            varStr.remove_prefix(varStr[2] == '-' ? 3 : 2);
            if (varStr.size() < 3)
               throw Exception("Tag is too short!", varStr);
            varStr.remove_suffix(varStr[varStr.size() - 3] == '-' ? 3 : 2);
            
            auto tokens = Expression::splitTokens(varStr);
            switch(tokens.size())
            {
            case 0:
               throw Exception("Variable definition without token!", varStr);
            case 1:
               block->body->nodeList.emplace_back(Variable(Expression::toToken(tokens[0])));
               break;
            default:
               block->body->nodeList.emplace_back(Variable(Expression::toToken(tokens[0]), Expression::toFilterChain(FilterFactoryT{}, tokens, 1)));
               break;
            }

            break;
         }
      }
   }
}
}

}

template<typename TagFactoryT = TagFactory, typename FilterFactoryT = FilterFactory>
Template parse(string_view content) {
   liquidpp::Template ast;
   ast.root.templateRange = content;
   
   try {
      impl::fastParser<TagFactoryT, FilterFactoryT>(content, ast.root);

      //auto itr = flatNodes.nodeList.begin();
      //ast.root = impl::buildBlocks<TagFactoryT, FilterFactoryT>(itr, flatNodes.nodeList.end());
      ast.root.templateRange = content;
   } catch(Exception& e) {
      e.position() = ast.findPosition(e.errorPart());
      throw;
   }

   return ast;
}
}

