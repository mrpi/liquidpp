#include "Expression.hpp"

#include "Context.hpp"

namespace liquidpp
{

boost::optional<Expression::Operator> Expression::toOperator(string_view operator_)
{
   if (operator_ == "==")
      return Operator::Equal;
   if (operator_ == "!=" || operator_ == "<>")
      return Operator::NotEqual;
   if (operator_ == "<")
      return Operator::Less;
   if (operator_ == "<=")
      return Operator::LessEqual;
   if (operator_ == ">")
      return Operator::Greater;
   if (operator_ == ">=")
      return Operator::GreaterEqual;
   if (operator_ == "contains")
      return Operator::Contains;
   if (operator_ == "and")
      return Operator::And;
   if (operator_ == "or")
      return Operator::Or;

   return boost::none;
}

namespace
{
inline bool isWhitespace(char c) {
   if (c == ' ')
      return true;
   if (c == '\t')
      return true;
   if (c == '\r')
      return true;
   if (c == '\n')
      return true;
   return false;
}

inline bool isOperatorChar(char c) {
   if (c == '=')
      return true;
   if (c == '!')
      return true;
   if (c == '<')
      return true;
   if (c == '>')
      return true;
   return false;
}

bool isQuotedString(string_view tokenStr)
{
   if (tokenStr.size() < 2)
      return false;

   auto first = tokenStr.front();
   auto last = tokenStr.back();

   if (first == last)
   {
      if (first == '\'' || first == '"')
         return true;
   }

   return false;
}

bool isInteger(string_view sv)
{
   if (sv.empty())
      return false;

   for (auto&& c : sv)
   {
      if (c >= '0' && c <= '9')
         ;
      else if (c == '-' && &c == &sv.front() && sv.size() > 1)
         ;
      else
         return false;
   }

   return true;
}

bool isFloat(string_view sv)
{
   if (sv.empty())
      return false;

   bool gotDot = false;

   for (auto&& c : sv)
   {
      if (c >= '0' && c <= '9')
         ;
      else if (c == '-' && &c == &sv.front() && sv.size() > 1)
         ;
      else if (c == '.' && &c != &sv.front() && !gotDot && sv.size() > 2)
         gotDot = true;
      else
         return false;
   }

   return true;
}

bool isOperator(const Expression::Token& token)
{
   return token.which() == 0;
}
}

std::vector<string_view> Expression::splitTokens(string_view sequence) {
   std::vector<string_view> res;
   const size_t len = sequence.size();
   if (len == 0)
      return res;

   enum class State
   {
      Whitespace,
      Token,
      Operator,
      DoubleQuoteString,
      SingleQuoteString
   };

   auto stateFromChar = [](char c){
      if (isWhitespace(c))
         return State::Whitespace;
      if (isOperatorChar(c))
         return State::Operator;
      if (c == '\'')
         return State::SingleQuoteString;
      if (c == '"')
         return State::DoubleQuoteString;
      return State::Token;
   };

   auto start = &sequence[0];
   auto finalizeToken = [&](const char* end){
      res.emplace_back(start, end-start);
      start = end;
   };

   State state = stateFromChar(*start);

   for(size_t i=0; i < len; i++)
   {
      auto& c = sequence[i];

      switch(state)
      {
         case State::Whitespace:
         {
            if (!isWhitespace(c)) {
               start = &c;
               state = stateFromChar(c);
            }

            break;
         }
         case State::Token:
         {
            if (isWhitespace(c) || isOperatorChar(c)) {
               finalizeToken(&c);
               state = stateFromChar(c);
            }

            break;
         }
         case State::Operator:
         {
            if (!isOperatorChar(c)) {
               finalizeToken(&c);
               state = stateFromChar(c);
            }

            break;
         }
         case State::DoubleQuoteString:
         {
            if (c == '"' && start != &c) {
               finalizeToken(&c+1);
               state = State::Whitespace;
            }
            break;
         }
         case State::SingleQuoteString:
         {
            if (c == '\'' && start != &c) {
               finalizeToken(&c+1);
               state = State::Whitespace;
            }
            break;
         }
      }
   }

   if (state == State::SingleQuoteString || state == State::DoubleQuoteString)
      throw std::runtime_error("Unterminated quoteed string!");
   if (state != State::Whitespace)
      finalizeToken(&sequence[0]+len);

   return res;
}

Expression::Token Expression::toToken(string_view tokenStr)
{
   if (tokenStr == "false")
      return Value::fromBool(false);
   else if (tokenStr == "true")
      return Value::fromBool(true);
   else if (isQuotedString(tokenStr))
   {
      tokenStr.remove_prefix(1);
      tokenStr.remove_suffix(1);
      return Value::reference(tokenStr);
   }
   else if (isInteger(tokenStr))
      return Value::fromIntegral(boost::lexical_cast<std::int64_t>(tokenStr));
   else if (isFloat(tokenStr))
      return Value::fromFloatingPoint(boost::lexical_cast<double>(tokenStr));

   auto opr = toOperator(tokenStr);
   if (opr)
      return *opr;

   return VariableName{tokenStr};
}

Expression Expression::fromSequence(string_view sequence)
{
   Expression res;
   auto splitted = splitTokens(sequence);
   res.tokens.reserve(splitted.size());

   bool first = true;
   for (auto&& token : splitted)
   {
      res.tokens.push_back(toToken(token));

      if (first)
         first = false;
      else
      {
         auto& prev = res.tokens[res.tokens.size()-2];
         auto& curr = res.tokens.back();
         if (isOperator(prev) && isOperator(curr))
            throw std::runtime_error("Operator may not follow on operator!");
         if (!isOperator(prev) && !isOperator(curr))
            throw std::runtime_error("Value may not follow on value (operator missing)!");
      }
   }

   if (res.tokens.empty())
      throw std::runtime_error("Expression has zero tokens!");
   if (isOperator(res.tokens.front()))
      throw std::runtime_error("Expression starts with operator!");
   if (isOperator(res.tokens.back()))
      throw std::runtime_error("Expression ends with operator!");

   return res;
}

Value Expression::value(const Context& c, const Token& t)
{
   switch(t.which())
   {
      case 0:
         throw std::runtime_error("Expected value or variable but got operator!");
      case 1:
         return boost::get<Value>(t);
      case 2:
         return c.get(boost::get<VariableName>(t).name);
   }

   throw std::runtime_error("Invalid state!");
}

bool Expression::matches(const Value& left, Operator operator_, const Value& right)
{
   switch(operator_)
   {
      case Operator::Equal:
         return left == right;
      case Operator::NotEqual:
         return left != right;
      case Operator::Less:
         return left < right;
      case Operator::LessEqual:
         return left <= right;
      case Operator::Greater:
         return left > right;
      case Operator::GreaterEqual:
         return left >= right;
      case Operator::Contains:
         return left.contains(right);
      case Operator::And:
      case Operator::Or:
         break;
   }

   throw std::runtime_error("Invalid operator for matching check!");
}

Value Expression::operator()(Context& c) const
{
   Value res;
   Operator lastOperator = Operator::Or;
   auto putToRes = [&](bool v){
      if (lastOperator == Operator::Or)
         res |= Value::fromBool(v);
      else
         res &= Value::fromBool(v);
   };

   const size_t cnt = tokens.size();
   const size_t last = cnt - 1;

   for(size_t i=0; i < cnt; i++)
   {
      auto& token = tokens[i];

      if (token.which() == 0)
      {
         lastOperator = boost::get<Operator>(token);
         continue;
      }

      if (i == last || (boost::get<Operator>(tokens[i+1]) == Operator::And || boost::get<Operator>(tokens[i+1]) == Operator::Or))
      {
         if (value(c, token))
            putToRes(true);
         else
            putToRes(false);
      }
      else
      {
         bool b = matches(value(c, tokens[i]), boost::get<Operator>(tokens[i+1]), value(c, tokens[i+2]));
         putToRes(b);
         i += 2;
      }
   }

   return res;
}

}
