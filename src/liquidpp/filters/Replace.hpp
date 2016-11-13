#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

#include <boost/algorithm/string.hpp>

namespace liquidpp
{
namespace filters
{

struct Replace
{
   Value operator()(Value&& val, Value&& toRemove, Value&& replacement) const
   {
      auto str = val.toString();

      boost::algorithm::replace_all(str, toRemove.toString(), replacement.toString());
      return std::move(str);
   }
};

}
}
