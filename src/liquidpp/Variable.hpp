#pragma once

#include "IRenderable.hpp"
#include "Expression.hpp"
#include "filters/Filter.hpp"
#include "Exception.hpp"

namespace liquidpp
{

struct Variable : public IRenderable {
   Expression::Token variable;
   std::vector<std::shared_ptr<filters::Filter>> filterChain;

   Variable() = default;

   template<typename FilterFactoryT>
   Variable(const FilterFactoryT& filterFac, string_view data)
   {
      data.remove_prefix(data[2] == '-' ? 3 : 2);
      data.remove_suffix(data[data.size() - 3] == '-' ? 3 : 2);

      auto tokens = Expression::splitTokens(data);
      if (tokens.empty())
         throw Exception("Variable definition without token!", data);
      variable = Expression::toToken(tokens[0]);

      std::shared_ptr<filters::Filter> currentFilter;
      bool newFilter = false;
      auto tokenCount = tokens.size();
      size_t attribIdx = 0;
      for (size_t i=1; i < tokenCount; i++)
      {
         auto&& token = tokens[i];
         if (token == "|")
         {
            if (newFilter)
               throw std::runtime_error("Unexpected second pipe character");
            newFilter = true;
            if (currentFilter)
               filterChain.push_back(std::move(currentFilter));
         }
         else if (newFilter)
         {
            currentFilter = filterFac(token);
            if (currentFilter == nullptr)
               throw Exception("Unknown filter!", token);
            newFilter = false;
         }
         else
         {
            if (currentFilter)
            {
               if (attribIdx++ % 2)
                  currentFilter->addAttribute(token);
               else
               {
                  if (attribIdx == 1) {
                     if (token != ":")
                        throw Exception("Expected ':' operator!", token);
                  }
                  else if (token != ",")
                     throw Exception("Expected ',' operator!", token);
               }
            }
            else
               throw Exception("Filter expression not starting with pipe symbol!", token);
         }
      }

      if (currentFilter)
         filterChain.push_back(std::move(currentFilter));
   }

   bool operator==(const Variable& other) const;

   void render(Context& context, std::string& out) const override final;
};

}
