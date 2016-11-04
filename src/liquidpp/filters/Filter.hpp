#pragma once

#include "../Accessor.hpp"
#include "../Exception.hpp"

namespace liquidpp
{
class Context;

namespace filters
{

struct Filter
{
   virtual ~Filter()
   {}

   virtual Value operator()(Context& c, Value&& val) const = 0;

   virtual void addAttribute(string_view sv)
   {
      throw Exception("This filter expects no attributes!", sv);
   }
};

}
}