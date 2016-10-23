#pragma once

#include "ValueConverter.hpp"
#include "config.h"

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

   struct VariableName {
      string_view name;

      bool operator==(const VariableName& other) const
      {
         return name == other.name;
      }
   };

   using RawTokens = SmallVector<string_view, 4>;
   using Token = boost::variant<Operator, Value, VariableName>;

   static bool matches(const Value& left, Operator operator_, const Value& right);
   static RawTokens splitTokens(string_view sequence);
   static Token toToken(string_view tokenStr);
   static Expression fromSequence(string_view sequence);
   static Value value(const Context& c, const Token& t);
   static bool isInteger(string_view sv);
   static bool isFloat(string_view sv);
   static bool isWhitespace(char c);

   Value operator()(Context& c) const;

   std::vector<Token> tokens;
};
}

