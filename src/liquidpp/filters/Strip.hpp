#pragma once

#include "Filter.hpp"

namespace liquidpp
{
namespace filters
{

struct Strip : public Filter
{
   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      auto sv = *val;

      auto pos = sv.find_first_not_of(" \t\r\n");
      if (pos == std::string::npos)
         return Value::reference(string_view{});
      sv.remove_prefix(pos);

      pos = sv.find_last_not_of(" \t\r\n");
      if (pos != std::string::npos)
         sv.remove_suffix(sv.size()-pos-1);

      return sv.to_string();
   }
};

}
}