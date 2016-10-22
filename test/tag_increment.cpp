#include "catch.hpp"

#include <liquidpp.hpp>

TEST_CASE("tag increment")
{
   liquidpp::Context c;

   auto rendered = liquidpp::render(R"({% assign var = 10 %}
{% increment var %}
{% increment var %}
{% increment var %}
{{ var }})", c);
   auto expected = R"(
0
1
2
10)";
   REQUIRE(rendered == expected);
}

TEST_CASE("tag decrement")
{
   liquidpp::Context c;

   auto rendered = liquidpp::render(R"({% assign variable = 10 %}
{% decrement variable %}
{% decrement variable %}
{% decrement variable %}
{{ variable }})", c);
   auto expected = R"(
-1
-2
-3
10)";
   REQUIRE(rendered == expected);
}
