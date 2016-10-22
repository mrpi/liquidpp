#pragma once

#include "Block.hpp"
#include "../config.h"
#include "../Expression.hpp"

#include <boost/optional.hpp>

namespace liquidpp {

template<bool Inverted>
struct Conditional : public Block {
   Expression expression;

   Conditional(Tag&& tag)
      : Block{std::move(tag)}, expression{Expression::fromSequence(value)} {
   }

   void render(Context& context, std::string& res, bool matches) const {
      if (matches)
      {
         for (auto&& node : body.nodeList)
         {
            if (isSpecificTag(node, "else") || isSpecificTag(node, "elsif"))
               break;

            renderNode(context, node, res);
         }
      }
      else
      {
         bool active = false;
         for (auto&& node : body.nodeList)
         {
            if (isSpecificTag(node, "else"))
            {
               if (active)
                  break;
               active = true;
               continue;
            }
            else if (isSpecificTag(node, "elsif"))
            {
               if (active)
                  break;
               auto elsIfExpression = Expression::fromSequence(boost::get<UnevaluatedTag>(node).value);
               if (elsIfExpression(context))
               {
                  active = true;
                  continue;
               }
            }

            if (active)
               renderNode(context, node, res);
         }
      }
   }

   void render(Context& context, std::string& res) const override final {
      if (expression(context))
         render(context, res, !Inverted);
      else
         render(context, res, Inverted);
   }
};

using If = Conditional<false>;
using Unless = Conditional<true>;
}