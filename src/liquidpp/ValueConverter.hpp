#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <map>

#include <boost/optional.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>

#include "config.h"

namespace liquidpp {
enum class ValueTag {
   Object,
   Range,
   Null,
   OutOfRange,
   SubValue
};

struct test {
   std::string s;
   string_view sv;
   long long i;
   double f;
   bool b;
   ValueTag t;
};

struct Value {
private:
   boost::variant<string_view, std::string, std::intmax_t, double, bool, ValueTag> data{ValueTag::Null};

public:
   Value() = default;

   Value(const Value&) = default;

   Value(Value&&) = default;

   Value& operator=(const Value&) = default;

   Value& operator=(Value&&) = default;

   Value(ValueTag tag)
      : data(tag) {}

   Value(const std::string& v)
      : data(v) {}

   Value(std::string&& v)
      : data(std::move(v)) {}

   static Value fromBool(bool b) {
      Value res;
      res.data = b;
      assert(res.isBool());
      return res;
   }

   template<typename T>
   static Value fromIntegral(T t) {
      Value res;
      res.data = static_cast<std::intmax_t>(t);
      assert(res.isIntegral());
      return res;
   }

   template<typename T>
   static Value fromFloatingPoint(T t) {
      Value res;
      res.data = static_cast<double>(t);
      assert(res.isFloatingPoint());
      return res;
   }

   static Value reference(string_view sv) {
      Value res;
      res.data = sv;
      return res;
   }

   bool isValueTag() const {
      return data.which() == 5;
   }

   bool isSimpleValue() const {
      return !isValueTag();
   }

   bool isNil() const {
      if (isValueTag()) {
         auto tagVal = boost::get<ValueTag>(data);
         if (tagVal == ValueTag::Null || tagVal == ValueTag::OutOfRange)
            return true;
      }
      return false;
   }

   bool isBool() const {
      return data.which() == 4;
   }

   bool isTrue() const {
      return isBool() && boost::get<bool>(data);
   }

   bool isFalse() const {
      return isBool() && (boost::get<bool>(data) == false);
   }

   bool isIntegral() const {
      return data.which() == 2;
   }

   bool isFloatingPoint() const {
      return data.which() == 3;
   }

   bool isNumber() const {
      return isIntegral() || isFloatingPoint();
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

   bool isStringViewRepresentable() const {
      return !isNumber();
   }

   Value& operator|=(const Value& v) {
      if (!*this)
         *this = v;
      return *this;
   }

   Value& operator&=(const Value& v) {
      if (*this)
         *this = v;
      return *this;
   }

   std::string toString() const {
      if (isIntegral())
         return boost::lexical_cast<std::string>(boost::get<std::int64_t>(data));
      else if (isFloatingPoint())
         return boost::lexical_cast<std::string>(boost::get<double>(data));
      else
         return (**this).to_string();
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
   template<typename Comparsion>
   struct Compare {
      template<typename T, typename U>
      bool operator()(const T& left, const U& right) const {
         return false;
      }

      template<typename T>
      bool operator()(const T& left, const T& right) const {
         return Comparsion{}(left, right);
      }

      bool operator()(const string_view& left, const std::string& right) const {
         return Comparsion{}(left, string_view{right});
      }

      bool operator()(const std::string& left, const string_view& right) const {
         return Comparsion{}(string_view{left}, right);
      }
   };

   template<template <typename> class C>
   bool compareWith(const Value& other) const
   {
      Compare<C<void>> comp;
      return boost::apply_visitor(comp, this->data, other.data);;
   }

public:
   bool operator==(const Value& other) const {
      return compareWith<std::equal_to>(other);
   }

   bool operator!=(const Value& other) const {
      return compareWith<std::not_equal_to>(other);
   }

   bool operator<(const Value& other) const {
      return compareWith<std::less>(other);
   }

   bool operator<=(const Value& other) const {
      return compareWith<std::less_equal>(other);
   }

   bool operator>(const Value& other) const {
      return compareWith<std::greater>(other);
   }

   bool operator>=(const Value& other) const {
      return compareWith<std::greater_equal>(other);
   }

   bool contains(const Value& other) const {
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

   bool operator!=(ValueTag tag) const {
      return !(*this == tag);
   }
};

using OptIndex = boost::optional<size_t>;
using ValueGetter = std::function<Value(OptIndex, string_view)>;

template<typename T>
struct ValueConverter : public std::false_type {
};

template<typename T>
constexpr bool hasValueConverter = ValueConverter<T>::value;

inline Value toValue(std::string v) {
   return Value(std::move(v));
}

inline Value toValue(const char* v) {
   return Value(std::string{v});
}

template<size_t Len>
Value toValue(const char (&v)[Len]) {
   return Value(std::string{v});
}

inline Value toValue(bool b) {
   return Value::fromBool(b);
}

template<typename T>
Value toValue(T t, std::enable_if_t<!hasValueConverter<T> && std::is_integral<T>::value, void**> = 0) {
   return Value::fromIntegral(t);
}

template<typename T>
Value toValue(T t, std::enable_if_t<!hasValueConverter<T> && std::is_floating_point<T>::value, void**> = 0) {
   return Value::fromFloatingPoint(t);
}

template<typename T>
std::string toValue(const T& value, std::enable_if_t<hasValueConverter<T>, void**> = 0) {
   return ValueConverter<T>::get(value);
}

namespace impl {
template<typename KeyT, typename ValueT>
struct AssociativeContainerConverter : public std::true_type {
   template<typename T>
   static auto get(T&& map) {
      return [map = std::forward<T>(map)](OptIndex
      idx, string_view
      path) -> Value
      {
         auto itr = map.find(boost::lexical_cast<KeyT>(path));
         if (itr != map.end())
            return toValue(itr->second);
         return ValueTag::Null;
      };
   }
};
}

template<typename KeyT, typename ValueT>
struct ValueConverter<std::map<KeyT, ValueT>> : public impl::AssociativeContainerConverter<KeyT, ValueT> {
};

template<typename KeyT, typename ValueT>
struct ValueConverter<std::unordered_map<KeyT, ValueT>> : public impl::AssociativeContainerConverter<KeyT, ValueT> {
};

template<typename ValueT>
struct ValueConverter<std::vector<ValueT>> : public std::true_type {
   template<typename T>
   static auto get(T&& vec) {
      return [vec = std::forward<T>(vec)](OptIndex
      idx, string_view
      path) -> Value
      {
         if (!idx)
            return ValueTag::Range;
         if (*idx < vec.size())
            return toValue(vec[*idx]);
         return ValueTag::OutOfRange;
      };
   }
};
}
