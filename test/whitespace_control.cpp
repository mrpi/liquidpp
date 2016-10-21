#include "catch.hpp"

#include <liquidpp.hpp>

TEST_CASE("Whitespace control: no whitespace stripping after tag")
{
   liquidpp::Context c;
   auto rendered = liquidpp::render(R"({% assign my_variable = "tomato" %}
{{ my_variable }})", c);
   auto expected = R"(
tomato)";
   REQUIRE(rendered == expected);
}

TEST_CASE("Whitespace control: whitespace stripping after tag")
{
   liquidpp::Context c;
   auto rendered = liquidpp::render(R"({%- assign my_variable = "tomato" -%}
{{ my_variable }})", c);
   auto expected = R"(tomato)";
   REQUIRE(rendered == expected);
}

TEST_CASE("Whitespace control: no whitespace stripping at all")
{
   liquidpp::Context c;
   auto rendered = liquidpp::render(R"({% assign username = "John G. Chalmers-Smith" %}
{% if username and username.size > 10 %}
  Wow, {{ username }}, you have a long name!
{% else %}
  Hello there!
{% endif %})", c);
   auto expected = R"(

  Wow, John G. Chalmers-Smith, you have a long name!
)";
   REQUIRE(rendered == expected);
}

TEST_CASE("Whitespace control: stripping all whitespaces")
{
   liquidpp::Context c;
   auto rendered = liquidpp::render(R"({%- assign username = "John G. Chalmers-Smith" -%}
{%- if username and username.size > 10 -%}
  Wow, {{ username }}, you have a long name!
{%- else -%}
  Hello there!
{%- endif -%})", c);
   auto expected = R"(Wow, John G. Chalmers-Smith, you have a long name!)";
   REQUIRE(rendered == expected);
}
