#pragma once

#include "config.h"
#include "Exception.hpp"
#include "Value.hpp"
#include "filters/Filter.hpp"

namespace liquidpp {
   
class Context;

struct Expression {
   enum class Operator {
      Equal,
      NotEqual,
      Less,
      LessEqual,
      Greater,
      GreaterEqual,
      Contains,
      And,
      Or
   };

   static boost::optional<Operator> toOperator(string_view str);

   using RawTokens = SmallVector<string_view, 4>;
   using Token = boost::variant<Operator, Value, Path>;
   
   struct FilterData
   {
      filters::Filter function{};
      SmallVector<Token, 1> args;
      
      explicit operator bool() const
      {
         return !!function;
      }
   };
   using FilterChain = SmallVector<FilterData, 1>;

   static bool matches(Context& c, const Value& left, Operator operator_, const Value& right, const Token& leftToken);
   static RawTokens splitTokens(string_view sequence);
   static Token toToken(string_view tokenStr);
   static Expression fromSequence(string_view sequence);
   static void assureIsSingleKeyPath(string_view rawToken);

   static Value value(Context& c, const Token& t, boost::optional<const FilterChain&> filterChain = boost::none);
   static std::tuple<Value, Path> value(Context& c, const RangeDefinition& range, size_t i, PathRef basePath);

   static bool isInteger(string_view sv);
   static bool isFloat(string_view sv);
   static bool isWhitespace(char c);
   static inline bool isAsciiAlpha(char c)
   {
      if (c >= 'a' && c <= 'z')
         return true;
      if (c >= 'A' && c <= 'Z')
         return true;
      return false;
   }
   
   static bool isDigit(char c);

   Value operator()(Context& c) const;

   template<typename FilterFactoryT>
   static FilterChain toFilterChain(const FilterFactoryT& filterFac, const RawTokens& tokens, size_t offset)
   {
      FilterChain filterChain;

      FilterData currentFilter;
      bool newFilter = false;
      auto tokenCount = tokens.size();
      size_t attribIdx = 0;
      for (size_t i=offset; i < tokenCount; i++)
      {
         auto&& token = tokens[i];
         if (token == "|")
         {
            if (newFilter)
               throw Exception("Unexpected second pipe character!", token);
            newFilter = true;
            if (currentFilter)
            {
               filterChain.push_back(std::move(currentFilter));
               currentFilter = FilterData{};
            }
         }
         else if (newFilter)
         {
            currentFilter.function = filterFac(token);
            attribIdx = 0;
            if (!currentFilter)
               throw Exception("Unknown filter!", token);
            newFilter = false;
         }
         else
         {
            if (currentFilter)
            {
               if (attribIdx++ % 2)
                  currentFilter.args.push_back(toToken(token));
               else
               {
                  if (attribIdx == 1) {
                     if (token != ":")
                        throw Exception("Expected ':' operator!", token);
                  }
                  else if (token != ",")
                     throw Exception("Expected ',' operator!", token);
                  else if (i == tokenCount-1)
                     throw Exception("Filter expression is unterminated (ending with ',')", token);
               }
            }
            else
               throw Exception("Filter expression not starting with pipe symbol!", token);
         }
      }

      if (currentFilter)
         filterChain.push_back(std::move(currentFilter));

      return filterChain;
   }

   std::vector<Token> tokens;
};
}

