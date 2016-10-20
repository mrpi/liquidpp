#include "Capture.hpp"

#include "../Context.hpp"

namespace liquidpp
{

Capture::Capture(Tag&& tag)
   : Block(std::move(tag)) {
   auto tokens = Expression::splitTokens(tag.value);
   if (tokens.size() != 1)
      throw Exception("Capture tag requires exactly once argument!", tag.value);

   variableName = tokens[0];
}

void Capture::render(Context& context, std::string& res) const
{
   if (body.nodeList.empty())
      context.setLiquidValue(variableName.to_string(), Value::reference(string_view{}));
   else
   {
      auto start = &boost::get<string_view>(body.nodeList.front()).front();
      auto end = &boost::get<string_view>(body.nodeList.back()).back() + 1;
      size_t len = end-start;
      context.setLiquidValue(variableName.to_string(), Value::reference(string_view{start, len}));
   }
}

}
