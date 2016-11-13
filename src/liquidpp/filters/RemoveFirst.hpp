#pragma once

#include "Filter.hpp"
#include "../Expression.hpp"

#include <boost/algorithm/string.hpp>

namespace liquidpp
{
namespace filters
{

struct RemoveFirst
{
   Value operator()(Value&& val, Value&& toRemove) const
   {
      auto str = val.toString();

      boost::algorithm::replace_first(str, toRemove.toString(), "");
      return std::move(str);
   }
};

}
}
