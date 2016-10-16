#pragma once

#include <liquidpp/Context.hpp>
#include <liquidpp/parser.hpp>

namespace liquidpp
{
template<typename TagFactoryT = TagFactory>
std::string render(string_view content, const Context& context)
{
   return parse<TagFactoryT>(content)(context);
}
}
