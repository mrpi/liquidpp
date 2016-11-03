#pragma once

#include "Filter.hpp"

namespace liquidpp
{
namespace filters
{

struct Capitalize : public Filter
{
   virtual Value operator()(Context& c, Value&& val) const override final
   {
      if (!val.isStringViewRepresentable())
         return std::move(val);

      auto res = val.toString();
      string_view sv = res;
      auto firstChar = utf8::popChar(sv);
      return boost::locale::to_title(firstChar.to_string(), c.locale()) + sv.to_string();
   }
};

}
}
