#pragma once

#include "Exception.hpp"
#include "config.h"

#include <limits>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace liquidpp {

using OptIndex = boost::optional<size_t>;
using KeyHolder = SmallVector<char, 64>;

struct Key {
private:
  boost::variant<string_view, size_t> mData;

public:
  explicit Key() {}

  explicit Key(string_view str) : mData(str) {}

  explicit Key(size_t str) : mData(str) {}

  explicit operator bool() const {
    if (mData.which() == 0)
      return !boost::get<string_view>(mData).empty();
    return true;
  }

  bool operator==(const Key &other) const { return mData == other.mData; }

  bool operator==(string_view keyName) const {
    if (isName())
      return name() == keyName;
    return false;
  }

  bool isName() const { return mData.which() == 0; }

  string_view name() const { return boost::get<string_view>(mData); }

  bool isIndex() const { return mData.which() == 1; }

  size_t index() const { return boost::get<size_t>(mData); }

#if 0
      KeyHolder qualifiedPath(string_view subPath) const
      {
         KeyHolder res;
         res.reserve(name.size() + 6 + subPath.size());
         res.insert(res.end(), name.begin(), name.end());

         if (idx)
         {
            res.push_back('[');
            //constexpr size_t MaxDigits = std::numeric_limits<size_t>::digits10 + 1;
            //res += boost::lexical_cast<std::array<char, MaxDigits>>(*idx).data();
            auto str = boost::lexical_cast<std::string>(*idx);
            res.insert(res.end(), str.begin(), str.end());
            res.push_back(']');
         }

         if (!subPath.empty())
         {
            res.push_back('.');
            res.insert(res.end(), subPath.begin(), subPath.end());
         }

         return res;
      }
#endif
};

using Path = SmallVector<Key, 8>;
using PathRef = gsl::span<const Key>;

inline Path operator+(Key key, PathRef tail) {
  Path res;
  res.reserve(tail.size() + 1);
  res.push_back(key);
  res.insert(res.end(), tail.begin(), tail.end());
  return res;
}

inline Path operator+(PathRef head, Key key) {
  Path res;
  res.reserve(head.size() + 1);
  res.insert(res.end(), head.begin(), head.end());
  res.push_back(key);
  return res;
}

inline Path operator+(PathRef head, PathRef tail) {
  Path res;
  res.reserve(head.size() + tail.size());
  res.insert(res.end(), head.begin(), head.end());
  res.insert(res.end(), tail.begin(), tail.end());
  return res;
}

inline Key popKey(PathRef &path) {
  Key res;

  if (!path.empty()) {
    res = path[0];
    path = path.subspan(1);
  }

  return res;
}

Key popKey(string_view &path);

inline Key popLastKey(PathRef &path) {
  Key res;

  if (!path.empty()) {
    auto idx = path.size() - 1;
    res = path[idx];
    path = path.subspan(0, idx);
  }

  return res;
}

inline Path toPath(string_view path) {
  Path res;

  while (auto key = popKey(path)) {
    res.push_back(key);
  }

  return res;
}
}
