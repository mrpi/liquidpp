#pragma once

namespace liquidpp
{
struct Comment : public Block
{
   Comment(Tag&& tag)
      : Block(std::move(tag))
   {}

   void render(Context& /*context*/, std::string& /*res*/) const override final
   {
   }
};
}