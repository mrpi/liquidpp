#include "Variable.hpp"

#include <tuple>

#include "Context.hpp"
#include "Expression.hpp"

namespace liquidpp
{

bool Variable::operator==(const Variable& other) const {
   auto tup = [](auto& var){ return std::tie(var.variable /*,TODO filters*/); };
   return  tup(*this) == tup(other);
}

void Variable::render(Context& context, std::string& out) const {
   auto&& val = Expression::value(context, variable);

   for (auto&& filter : filterChain)
      val = (*filter)(context, std::move(val));

   if (val.isStringViewRepresentable())
   {
      auto sv = *val;
      out.append(sv.data(), sv.size());
   }
   else
      out += val.toString();
}
}
