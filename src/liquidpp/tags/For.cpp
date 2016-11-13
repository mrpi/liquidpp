#include "For.hpp"

#include "../Context.hpp"

namespace liquidpp {
For::For(Tag &&tag) : Block(std::move(tag)) {
  auto tokens = Expression::splitTokens(value);
  if (tokens.size() < 3)
    throw Exception("Not enough parameters in 'for' tag!", value);

  loopVariable = tokens[0];
  if (tokens[1] != "in")
    throw Exception("Second token in 'for' tag has to be the 'in' keyword!",
                    tokens[1]);
  rangeExpression = toRangeDefinition(tokens[2]);
  if (!rangeExpression)
    rangePath = toPath(tokens[2]);

  auto cnt = tokens.size();
  for (size_t i = 3; i < cnt; i++) {
    auto &key = tokens[i];

    if (key == "limit") {
      if (i + 2 < cnt) {
        if (tokens[i + 1] != ":")
          throw Exception("Expected operator ':' after parameter name!",
                          tokens[i + 1]);
        limitToken = Expression::toToken(tokens[i + 2]);
        i += 2;
      } else
        throw Exception("Not enough arguments for this parameter!", key);
    } else if (key == "offset") {
      if (i + 2 < cnt) {
        if (tokens[i + 1] != ":")
          throw Exception("Expected operator ':' after parameter name!",
                          tokens[i + 1]);
        offsetToken = Expression::toToken(tokens[i + 2]);
        i += 2;
      } else
        throw Exception("Not enough arguments for this parameter!", key);
    } else if (key == "reversed")
      reversed = true;
    else
      throw Exception("Unknown parameter on 'for' tag!", key);
  }
}

boost::optional<For::RangeExpression> For::toRangeDefinition(string_view sv) {
  if (sv.front() == '(') {
    if (sv.back() != ')')
      throw Exception("Unterminated range expression!", sv);

    sv.remove_prefix(1);
    sv.remove_suffix(1);

    RangeExpression res;
    auto idx = sv.find("..");
    if (idx == std::string::npos)
      throw Exception("Range expression misses '..'!", sv);
    res.startIdxToken = Expression::toToken(sv.substr(0, idx));
    res.endIdxToken = Expression::toToken(sv.substr(idx + 2));

    return res;
  }

  return boost::none;
}

Value For::LoopData::get(PathRef path) const {
  auto key = popKey(path);
  if (!key)
    return ValueTag::Object;
  if (!path.empty())
    return ValueTag::SubValue;

  if (key == "first")
    return toValue(idx == 0);
  if (key == "index")
    return toValue(idx + 1);
  if (key == "index0")
    return toValue(idx);
  if (key == "last")
    return toValue(idx + 1 == size);
  if (key == "length")
    return toValue(size);
  if (key == "rindex")
    return toValue(size - idx);
  if (key == "rindex0")
    return toValue(size - idx - 1);

  return ValueTag::Null;
}

struct RecursiveDepth {
  size_t &depth;
  const size_t pattern;

  explicit RecursiveDepth(size_t &d)
      : depth(d), pattern((1 << (10 - depth)) - 1) {
    depth++;
  }

  bool checkRequired(size_t i) const {
    if (depth > 10)
      return true;

    if ((i & pattern) == pattern)
      return true;
    return false;
  }

  ~RecursiveDepth() { depth--; }
};

void For::render(Context &context, std::string &res) const {
  size_t rangeExprStart = 0;
  size_t rangeExprEnd = 0;
  Value val;

  if (rangeExpression) {
    rangeExprStart = static_cast<size_t>(
        Expression::value(context, rangeExpression->startIdxToken)
            .integralValue());
    rangeExprEnd = static_cast<size_t>(
        Expression::value(context, rangeExpression->endIdxToken)
            .integralValue());

    if (rangeExprStart > rangeExprEnd)
      throw Exception("Start index of range is larger than its end!", value);
  } else
    val = context.get(rangePath);

  size_t size = 0;
  if (rangeExpression)
    size = rangeExprEnd - rangeExprStart + 1;
  else if (val.isRange())
    size = val.size();
  else if (val.isSimpleValue())
    size = 1;

  auto getValue = [&](size_t i) mutable {
    if (rangeExpression)
      return std::make_tuple(toValue(rangeExprStart + i), Path{});
    else if (val.isSimpleValue())
      return std::make_tuple(val, Path{});
    else if (val.isRange())
      return Expression::value(context, val.range(), i, rangePath);

    return std::make_tuple(Value{ValueTag::OutOfRange}, Path{});
  };

  size_t offset = 0;
  if (offsetToken)
    offset = static_cast<size_t>(
        Expression::value(context, *offsetToken).integralValue());

  size_t limit = size - offset;
  if (limitToken) {
    auto l = static_cast<size_t>(
        Expression::value(context, *limitToken).integralValue());
    if (l < limit)
      limit = l;
  }

  if (limit == 0) {
    bool elseFound = false;
    for (auto &&node : body.nodeList) {
      if (isSpecificTag(node, "else"))
        elseFound = true;
      else if (elseFound)
        renderNode(context, node, res);
    }
    return;
  }

  auto beforOutputSize = res.size();
  RecursiveDepth depth{context.recursiveDepth()};
  for (size_t i = 0; i < limit; i++) {
    size_t idx = i + offset;
    if (reversed)
      idx = size - offset - i - 1;

    auto itm = getValue(idx);
    if (!renderElement(context, res, std::get<0>(itm), std::get<1>(itm),
                       LoopData{i, limit}))
      break;

    auto resSize = res.size();
    if (resSize > context.maxOutputSize())
      throw Exception("Maximal output size reached!", name);
    if (depth.checkRequired(i)) {
      auto writtenInLoop = resSize - beforOutputSize;
      if (writtenInLoop < (context.minOutputPer1024Loops() / depth.depth))
        throw Exception("Long running loop with only few output detected!",
                        name);
    }
  }
}

template <> struct Accessor<For::LoopData> : public std::true_type {
  template <typename T> 
  static Value get(const T& loopData, PathRef path) {
     return loopData.get(path);
  }
};

bool For::renderElement(Context &context, std::string &res,
                        const Value &currentVal, PathRef idxPath,
                        LoopData forLoop) const {
  Context loopVarContext(&context);
  loopVarContext.set("forloop", forLoop);

  if (currentVal.isSimpleValue())
    loopVarContext.setLiquidValue(loopVariable.to_string(), currentVal);
  else if (currentVal != ValueTag::OutOfRange)
    loopVarContext.setLink(loopVariable.to_string(), idxPath);
  else
    return false;

  try {
    for (auto &&node : body.nodeList) {
      if (isSpecificTag(node, "else"))
        break;
      renderNode(loopVarContext, node, res);
    }

    return true;
  } catch (DoContinue &) {
    return true;
  } catch (DoBreak &) {
    return false;
  }
}
}
