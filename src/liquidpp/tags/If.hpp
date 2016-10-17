#pragma once

#include "Block.hpp"

namespace liquidpp {

struct If : public Block {
   std::string variable;
   struct CompareWith {
      std::string operator_;
      std::string value;
   };
   CompareWith compareWith;

   static bool operatorIsValid(const std::string& operator_);

   If(Tag&& tag);

   friend std::ostream& operator<<(std::ostream& os, const If& con);

   void render(Context& context, std::string& res) const override final;

};
}