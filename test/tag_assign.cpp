#include "catch.hpp"

#include <liquidpp.hpp>

#include <liquidpp/tags/Assign.hpp>

auto templ = R"(

)";

TEST_CASE("render assign")
{
   std::vector<int> v{1, 2, 23, 42};
   liquidpp::Context c;
   c.set("var", "x");
   c.set("range", v);

   {
      auto rendered = liquidpp::render("{% assign tmp = 1 %}{{tmp}}", c);
      REQUIRE(rendered == "1");
   }

   {
      auto rendered = liquidpp::render("{{tmp}}", c);
      REQUIRE(rendered == "");
   }

   {
      auto rendered = liquidpp::render("{% assign tmp = range %}{{tmp[3]}}", c);
      REQUIRE(rendered == "42");
   }
}
