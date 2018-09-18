#pragma once

#include "Filter.hpp"

namespace liquidpp
{
namespace filters
{

struct Rstrip
{
   Value operator()(Value&& val) const
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      auto sv = *val;
      auto pos = sv.find_last_not_of(" \t\r\n");
      if (pos != std::string::npos)
         return to_string(sv.substr(0, pos+1));

      return Value::reference(string_view{});
   }
};

}
}