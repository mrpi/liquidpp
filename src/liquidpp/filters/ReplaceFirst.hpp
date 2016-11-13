#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

#include <boost/algorithm/string.hpp>

namespace liquidpp
{
namespace filters
{

struct ReplaceFirst
{
   Value operator()(Value&& val, Value&& toRemove, Value&& replacement) const
   {
      auto str = val.toString();

      boost::algorithm::replace_first(str, toRemove.toString(), replacement.toString());
      return std::move(str);
   }
};

}
}
