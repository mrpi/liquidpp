#pragma once

#include <boost/property_tree/ptree_fwd.hpp>

namespace liquidpp {
template <>
struct ValueConverter<boost::property_tree::ptree> : public std::true_type {
  template <typename T> static auto get(T &&propTree) {
    return [propTree = std::forward<T>(propTree)](PathRef path)->Value {
      auto parent = propTree;

      while (const auto key = popKey(path)) {
        if (key.isName()) {
          auto child = propTree.get_child_optional(key.name().to_string());
          if (!child) {
            if (path.empty()) {
              auto res = parent.template get_optional<std::string>(
                  key.name().to_string());
              if (res)
                return *res;
            }

            return ValueTag::Null;
          }
          parent = *child;
        } else {
          size_t i = 0;
          for (auto &&c : parent) {
            if (i++ == key.index()) {
              parent = c.second;
              break;
            }
          }
          if (i <= key.index())
            return ValueTag::OutOfRange;
          continue;
        }
      }

      if (parent.empty()) {
        auto res = parent.template get_value_optional<std::string>();
        if (res)
          return *res;

        return ValueTag::Null;
      } else
        return RangeDefinition{parent.size()};
    };
  }
};
}
