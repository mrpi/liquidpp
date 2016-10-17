#include "Variable.hpp"

#include <tuple>

#include "Context.hpp"

namespace liquidpp
{

bool Variable::operator==(const Variable& other) const {
   auto tup = [](auto& var){ return std::tie(var.leftPadding, var.variableName, var.filters); };
   return  tup(*this) == tup(other);
}

void Variable::render(Context& context, std::string& out) const {
   auto&& val = context.get(variableName);
   if (val)
      out += *val;
}
}
