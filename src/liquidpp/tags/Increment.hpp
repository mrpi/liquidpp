#pragma once

#include "Block.hpp"
#include "../config.h"

namespace liquidpp
{

template<int Step, int Initial>
struct IncrementBase : public Tag
{
   std::string keyName;

   static std::string generateKeyName(string_view variableName)
   {
      std::string var;
      var.reserve(variableName.size());
      var += '\0';
      var += static_cast<char>(Step);
      var += static_cast<char>(Initial);
      var.append(variableName.data(), variableName.size());
      return var;
   }

   IncrementBase(Tag&& tag)
     : Tag(std::move(tag))
   {
      auto tokens = Expression::splitTokens(value);
      if (tokens.size() != 1)
         throw Exception("Invalid parameter count!", value);

      keyName = generateKeyName(tokens[0]);
   }

   void render(Context& context, std::string& res) const override final
   {
      auto& dsc = context.documentScopeContext();

      std::intmax_t numVal = Initial;
      auto val = dsc.get(keyName);
      if (val.isIntegral())
         numVal = val.integralValue();

      res += boost::lexical_cast<std::string>(numVal);
      numVal += Step;
      dsc.set(keyName, numVal);
   }
};

using Increment = IncrementBase<1, 0>;
using Decrement = IncrementBase<-1, -1>;

}