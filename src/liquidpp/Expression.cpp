#include "Expression.hpp"

#include "Context.hpp"
#include "Exception.hpp"

#include "filters/Filter.hpp"

namespace liquidpp {

boost::optional<Expression::Operator>
Expression::toOperator(string_view operator_) {
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

bool Expression::isWhitespace(char c) {
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

namespace {
inline bool isOperatorChar(char c) {
  if (c == '=')
    return true;
  if (c == '!')
    return true;
  if (c == '<')
    return true;
  if (c == '>')
    return true;
  if (c == '|') // start filter
    return true;
  if (c == ':') // in filters
    return true;
  if (c == ',') // in filters
    return true;
  return false;
}

#if 0
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
#endif

bool isOperator(const Expression::Token &token) { return token.which() == 0; }
}

bool Expression::isDigit(char c) {
  if (c >= '0' && c <= '9')
    return true;
  return false;
}

bool Expression::isInteger(string_view sv) {
  if (sv.empty())
    return false;

  for (auto &&c : sv) {
    if (isDigit(c))
      ;
    else if (c == '-' && &c == &sv.front() && sv.size() > 1)
      ;
    else
      return false;
  }

  return true;
}

bool Expression::isFloat(string_view sv) {
  if (sv.empty())
    return false;

  bool gotDot = false;

  for (auto &&c : sv) {
    if (isDigit(c))
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

Expression::RawTokens Expression::splitTokens(string_view sequence) {
  RawTokens res;
  const size_t len = sequence.size();
  if (len == 0)
    return res;

  enum class State {
    Whitespace,
    Variable,
    Operator,
    DoubleQuoteString,
    SingleQuoteString
  };

  auto stateFromChar = [](char c) {
    if (isWhitespace(c))
      return State::Whitespace;
    if (isOperatorChar(c))
      return State::Operator;
    if (c == '\'')
      return State::SingleQuoteString;
    if (c == '"')
      return State::DoubleQuoteString;
    return State::Variable;
  };

  auto validateVariableName = [](string_view varName) {
    auto firstChar = varName.front();
    if (firstChar == '.' || firstChar == '[' || firstChar == ']')
      throw Exception("Begin of variable name is invalid!", varName);

    auto lastChar = varName.back();
    if (lastChar == '.' || lastChar == '[' || lastChar == '"' ||
        lastChar == '\'')
      throw Exception("Variable name is incomplete!", varName);
  };

  auto start = &sequence[0];
  State state = stateFromChar(*start);
  auto finalizeToken = [&](const char *end) {
    res.emplace_back(start, end - start);
    if (state == State::Variable)
      validateVariableName(res.back());
    start = end;
  };

  for (size_t i = 0; i < len; i++) {
    auto &c = sequence[i];

    switch (state) {
    case State::Whitespace: {
      if (!isWhitespace(c)) {
        start = &c;
        state = stateFromChar(c);
      }

      break;
    }
    case State::Variable: {
      if (isWhitespace(c) || isOperatorChar(c)) {
        finalizeToken(&c);
        state = stateFromChar(c);
      }
      if (c == '"' || c == '\'') {
        auto pos = sequence.find(c, i + 1);
        if (pos == std::string::npos)
          throw Exception("Unterminated quoted string in variable definition!",
                          sequence.substr(i));
        i = pos;
      }

      break;
    }
    case State::Operator: {
      if (!isOperatorChar(c)) {
        finalizeToken(&c);
        state = stateFromChar(c);
      }

      break;
    }
    case State::DoubleQuoteString: {
      if (c == '"' && start != &c) {
        finalizeToken(&c + 1);
        state = State::Whitespace;
      }
      break;
    }
    case State::SingleQuoteString: {
      if (c == '\'' && start != &c) {
        finalizeToken(&c + 1);
        state = State::Whitespace;
      }
      break;
    }
    }
  }

  if (state == State::SingleQuoteString || state == State::DoubleQuoteString)
    throw Exception("Unterminated quoted string!", string_view{start - 1, 1});
  if (state != State::Whitespace)
    finalizeToken(&sequence[0] + len);

  return res;
}

Expression::Token Expression::toToken(string_view tokenStr) {
  switch (tokenStr[0]) {
  case '\0':
    throw Exception("Zero char ('\\0') is not allowed at this position!",
                    tokenStr);
  case 'a':
    if (tokenStr == "and")
      return Operator::And;
    break;
  case 'c':
    if (tokenStr == "contains")
      return Operator::Contains;
    break;
  case 'f':
    if (tokenStr == "false")
      return Value{false};
    break;
  case 'o':
    if (tokenStr == "or")
      return Operator::Or;
    break;
  case 't':
    if (tokenStr == "true")
      return Value{true};
    break;
  case '\'':
  case '"':
    tokenStr.remove_prefix(1);
    tokenStr.remove_suffix(1);
    return Value::reference(tokenStr);
  case '-':
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    if (isInteger(tokenStr))
      return Value{lex_cast<std::intmax_t>(tokenStr)};
    else if (isFloat(tokenStr))
      return Value{lex_cast<double>(tokenStr)};
    else
      throw Exception("Token is starting with a digit but not a valid number!",
                      tokenStr);
  case '<':
  case '>':
  case '=':
  case '!': {
    auto opr = toOperator(tokenStr);
    if (!opr)
      throw Exception("Unknown operator!", tokenStr);
    return *opr;
  }
  }

  return toPath(tokenStr);
}

Expression Expression::fromSequence(string_view sequence) {
  Expression res;
  auto splitted = splitTokens(sequence);
  res.tokens.reserve(splitted.size());

  bool first = true;
  for (auto &&token : splitted) {
    res.tokens.push_back(toToken(token));

    if (first)
      first = false;
    else {
      auto &prev = res.tokens[res.tokens.size() - 2];
      auto &curr = res.tokens.back();
      if (isOperator(prev) && isOperator(curr))
        throw Exception("Operator may not follow on operator!", token);
      if (!isOperator(prev) && !isOperator(curr))
        throw Exception("Value may not follow on value (operator missing)!",
                        token);
    }
  }

  if (res.tokens.empty())
    throw Exception("Expression has zero tokens!", sequence);
  if (isOperator(res.tokens.front()))
    throw Exception("Expression starts with operator!", sequence);
  if (isOperator(res.tokens.back()))
    throw Exception("Expression ends with operator!", sequence);

  return res;
}

void Expression::assureIsSingleKeyPath(string_view rawToken)
{
  auto loopVarToken = Expression::toToken(rawToken);
  if (loopVarToken.which() != 2)
    throw Exception("Loop variable is not a path!", rawToken);
  auto& path = boost::get<Path>(loopVarToken);
  if (path.size() != 1)
    throw Exception("Loop variable has to be a single key (no dots or array indizes allowed)!", rawToken);
}

std::tuple<Value, Path> Expression::value(Context &c,
                                          const RangeDefinition &range,
                                          size_t i, PathRef basePath) {
  assert(i < range.size());

  if (range.usesInlineValues())
    return std::make_tuple(Value{range.inlineValue(i)}, Path{});
  else {
    auto subPath = basePath + Key{range.index(i)};
    return std::make_tuple(c.get(subPath), subPath);
  }
}

namespace {
RangeDefinition &inlineRangeValues(Context &c, RangeDefinition &r,
                                   PathRef basePath) {
  if (r.usesInlineValues())
    return r;

  RangeDefinition::InlineValues res;
  res.reserve(r.size());
  for (size_t i = 0; i < r.size(); i++) {
    Value v = std::get<0>(Expression::value(c, r, i, basePath));
    if (v == ValueTag::Object) {
      r.setRangePath(basePath);
      return r;
    }
    res.push_back(v.toString());
  }

  r = RangeDefinition{std::move(res), basePath};
  return r;
}
}

Value Expression::value(Context &c, const Token &t,
                        boost::optional<const FilterChain &> filterChain) {
  Value res;
  PathRef path;

  switch (t.which()) {
  case 0:
    throw std::runtime_error("Expected value or variable but got operator!");
  case 1:
    res = boost::get<Value>(t);
    break;
  case 2:
    path = boost::get<Path>(t);
    res = c.get(path);
    break;
  }

  if (filterChain) {
    for (auto &&filter : *filterChain) {
      if (res.isRange())
        inlineRangeValues(c, res.range(), path);

      filters::FilterArgs args;
      args.reserve(filter.args.size());
      for (auto&& arg : filter.args)
         args.push_back(value(c, arg));
      
      res = filter.function(c, std::move(res), std::move(args));
    }
  }

  return res;
}

bool Expression::matches(Context &c, const Value &left, Operator operator_,
                         const Value &right, const Token &leftToken) {
  switch (operator_) {
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
    if (left.isRange()) {
      auto &&range = left.range();
      if (!range.usesInlineValues()) {
        auto cnt = range.size();
        auto &basePath = boost::get<Path>(leftToken);
        for (size_t i = 0; i < cnt; i++) {
          auto val = std::get<0>(value(c, range, i, basePath));
          if (val == right)
            return true;
        }

        return false;
      }
    }
    return left.contains(right);
  case Operator::And:
  case Operator::Or:
    break;
  }

  throw std::runtime_error("Invalid operator for matching check!");
}

Value Expression::operator()(Context &c) const {
  Value res;
  Operator lastOperator = Operator::Or;
  auto putToRes = [&](bool v) {
    if (lastOperator == Operator::Or)
      res |= Value{v};
    else
      res &= Value{v};
  };

  const size_t cnt = tokens.size();
  const size_t last = cnt - 1;

  for (size_t i = 0; i < cnt; i++) {
    auto &token = tokens[i];

    if (token.which() == 0) {
      lastOperator = boost::get<Operator>(token);
      continue;
    }

    if (i == last || (boost::get<Operator>(tokens[i + 1]) == Operator::And ||
                      boost::get<Operator>(tokens[i + 1]) == Operator::Or)) {
      if (value(c, token))
        putToRes(true);
      else
        putToRes(false);
    } else {
      bool b =
          matches(c, value(c, tokens[i]), boost::get<Operator>(tokens[i + 1]),
                  value(c, tokens[i + 2]), tokens[i]);
      putToRes(b);
      i += 2;
    }
  }

  return res;
}
}
