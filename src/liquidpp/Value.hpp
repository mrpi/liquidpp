#pragma once

#include <iomanip>

#include "config.h"
#include "Key.hpp"

namespace liquidpp
{

enum class ValueTag { Object, Null, OutOfRange, SubValue };

class RangeDefinition {
public:
  using GaplessIndices = std::pair<size_t, size_t>;
  using AvailableIndices = SmallVector<size_t, 8>;
  using InlineValues = SmallVector<std::string, 2>;

private:
  boost::variant<GaplessIndices, AvailableIndices, InlineValues> data;
  PathRef mRangePath;

public:
  explicit RangeDefinition(size_t size) : data{GaplessIndices{0, size}} {}

  // Use this constructor if your range does not start naturally with index 0
  // (or if you are working on a sub range)
  RangeDefinition(size_t beginIdx, size_t endIdx)
      : data{GaplessIndices{beginIdx, endIdx}} {
    assert(beginIdx >= endIdx);
  }

  explicit RangeDefinition(AvailableIndices availableIndices)
      : data{std::move(availableIndices)} {}

  explicit RangeDefinition(InlineValues inlineValues,
                           PathRef rangePath = PathRef{})
      : data{std::move(inlineValues)}, mRangePath{rangePath} {}

  const std::string &inlineValue(size_t i) const {
    return boost::get<InlineValues>(data)[i];
  }

  PathRef rangePath() const { return mRangePath; }

  void setRangePath(PathRef val) { mRangePath = val; }

  const InlineValues &inlineValues() const {
    return boost::get<InlineValues>(data);
  }

  InlineValues &inlineValues() { return boost::get<InlineValues>(data); }

  bool usesInlineValues() const { return data.which() == 2; }

  size_t index(size_t i) const {
    switch (data.which()) {
    case 0:
      return boost::get<GaplessIndices>(data).first + i;
    case 1:
      return boost::get<AvailableIndices>(data)[i];
    }

    throw std::runtime_error("This range has no index type (inline values)!");
  }

  size_t size() const {
    switch (data.which()) {
    case 0: {
      auto &&r = boost::get<GaplessIndices>(data);
      return r.second - r.first;
    }
    case 1:
      return boost::get<AvailableIndices>(data).size();
    case 2:
      return boost::get<InlineValues>(data).size();
    }

    throw std::runtime_error("Invalid state (unknown range type)!");
  }

  bool operator==(const RangeDefinition &other) const {
    return data == other.data;
  }
};

class Value {
private:
  boost::variant<string_view, std::string, std::intmax_t, double, bool,
                 RangeDefinition, ValueTag>
      data{ValueTag::Null};

public:
  Value() = default;

  Value(const Value &) = default;
  Value(Value &&) = default;
  
  template<typename T>
  Value(T val, std::enable_if_t<std::is_integral<T>::value && !std::is_same<std::decay_t<T>, bool>::value, void**> = 0)
    : data(static_cast<std::intmax_t>(val))
  {
    assert(isIntegral());
  }
  
  template<typename T>
  Value(T val, std::enable_if_t<std::is_floating_point<T>::value, void**> = 0)
    : data(static_cast<double>(val))
  {
    assert(isFloatingPoint());
  }
  
  template<typename T>
  Value(T val, std::enable_if_t<std::is_same<std::decay_t<T>, bool>::value, void**> = 0)
    : data(val)
  {
    assert(isBool());
  }
  
  Value(ValueTag val)
    : data(val)
  {
     assert(isValueTag());
  }

  Value(RangeDefinition rangeDef) : data(std::move(rangeDef)) {}

  Value(const std::string &v) : data(v) {}

  Value(std::string &&v) : data(std::move(v)) {}

  Value &operator=(const Value &) = default;
  Value &operator=(Value &&) = default;

  static Value reference(string_view sv) {
    Value res;
    res.data = sv;
    return res;
  }
  
  Value asReference() const {
    if (data.which() == 1)
      return reference(boost::get<std::string>(data));
    return *this;
  }

  bool isRange() const { return data.which() == 5; }

  const RangeDefinition &range() const {
    return boost::get<RangeDefinition>(data);
  }

  RangeDefinition &range() { return boost::get<RangeDefinition>(data); }

  size_t size() const {
    if (isRange())
      return boost::get<RangeDefinition>(data).size();
    if (isStringViewRepresentable()) {
      string_view sv = **this;
      return sv.size();
    }

    // count of (printed) decimals of numbers
    return toString().size();
  }

  bool isValueTag() const { return data.which() == 6; }

  bool isSimpleValue() const { return !isValueTag() && !isRange(); }

  bool isNil() const {
    if (isValueTag()) {
      auto tagVal = boost::get<ValueTag>(data);
      if (tagVal == ValueTag::Null || tagVal == ValueTag::OutOfRange ||
          tagVal == ValueTag::SubValue)
        return true;
    }
    return false;
  }

  bool isBool() const { return data.which() == 4; }

  bool isTrue() const { return isBool() && boost::get<bool>(data); }

  bool isFalse() const { return isBool() && (boost::get<bool>(data) == false); }

  bool isIntegral() const { return data.which() == 2; }

  std::intmax_t integralValue() const {
    return boost::get<std::intmax_t>(data);
  }

  bool isFloatingPoint() const { return data.which() == 3; }

  double floatingPointValue() const { return boost::get<double>(data); }

  bool isNumber() const { return isIntegral() || isFloatingPoint(); }

  bool isStringView() const {
    return data.which() == 0;
  }

  bool isStringType() const {
    if (data.which() == 0 || data.which() == 1)
      return true;
    return false;
  }

  explicit operator bool() const {
    if (isFalse() || isNil())
      return false;
    return true;
  }

  bool operator!() const {
    if (*this)
      return false;
    return true;
  }

  bool isStringViewRepresentable() const { return !isNumber(); }

  Value &operator|=(const Value &v) {
    if (!*this)
      *this = v;
    return *this;
  }

  Value &operator&=(const Value &v) {
    if (*this)
      *this = v;
    return *this;
  }

  std::string toString() const {
    if (isIntegral())
      return boost::lexical_cast<std::string>(boost::get<std::intmax_t>(data));
    else if (isFloatingPoint()) {
      std::ostringstream oss;
      oss << std::setprecision(16) << boost::get<double>(data);
      return oss.str();
    } else
      return to_string((**this));
  }

  string_view operator*() const {
    // TODO: static_visitor
    if (data.which() == 0)
      return boost::get<string_view>(data);
    if (data.which() == 1)
      return boost::get<std::string>(data);
    if (isBool())
      return isTrue() ? "true" : "false";
    return string_view{};
  }

private:
  template <typename Comparsion>
  struct Compare : public boost::static_visitor<bool> {
    template <typename T, typename U>
    bool operator()(const T &left, const U &right) const {
      return false;
    }

    template <typename T> bool operator()(const T &left, const T &right) const {
      return Comparsion{}(left, right);
    }

    bool operator()(const string_view &left, const std::string &right) const {
      return Comparsion{}(left, string_view{right});
    }

    bool operator()(const std::string &left, const string_view &right) const {
      return Comparsion{}(string_view{left}, right);
    }

    bool operator()(const double &left, const std::intmax_t &right) const {
      return Comparsion{}(left, static_cast<double>(right));
    }

    bool operator()(const std::intmax_t &left, const double &right) const {
      return Comparsion{}(static_cast<double>(left), right);
    }

    bool operator()(const RangeDefinition &left,
                    const RangeDefinition &right) const {
      return false;
    }
  };

  template <template <typename> class C>
  bool compareWith(const Value &other) const {
    Compare<C<void>> comp;
    return boost::apply_visitor(comp, this->data, other.data);
  }

public:
  bool operator==(const Value &other) const {
    return compareWith<std::equal_to>(other);
  }

  bool operator!=(const Value &other) const {
    return compareWith<std::not_equal_to>(other);
  }

  bool operator<(const Value &other) const {
    return compareWith<std::less>(other);
  }

  bool operator<=(const Value &other) const {
    return compareWith<std::less_equal>(other);
  }

  bool operator>(const Value &other) const {
    return compareWith<std::greater>(other);
  }

  bool operator>=(const Value &other) const {
    return compareWith<std::greater_equal>(other);
  }

  bool contains(const Value &other) const {
    if (isRange()) {
      auto needle = other.toString();
      for (auto &&val : range().inlineValues()) {
        if (val == needle)
          return true;
      }

      return false;
    }

    if (!isStringType())
      return false;

    auto thisStr = **this;
    auto otherStr = other.toString();
    return thisStr.find(otherStr) != std::string::npos;
  }

  bool operator==(ValueTag tag) const {
    if (isValueTag())
      return boost::get<ValueTag>(data) == tag;
    return false;
  }

  bool operator!=(ValueTag tag) const { return !(*this == tag); }
};

inline Value operator||(const Value &left, const Value& right) {
   auto res = left;
   res |= right;
   return res;
}

inline Value operator&&(const Value &left, const Value& right) {
   auto res = left;
   res &= right;
   return res;
}

inline std::ostream &operator<<(std::ostream &os, const Value &v) {
  if (v.isNumber())
    os << v.toString();
  else
    os << '"' << v.toString() << '"';
  return os;
}
   
}
