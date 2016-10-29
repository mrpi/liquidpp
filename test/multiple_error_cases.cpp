#include "catch.hpp"

#include <liquidpp.hpp>

TEST_CASE("stackoverflow with array name ending with '.'")
{
   liquidpp::Context c;

   // reduced case found with libFuzzer
   auto templ = R"(
{% assign handle="cs  ake, cookie, foo" | split: "" %}
{% for h in handle.%}
 {{h}}
{% endfor %}
   )";
   
   REQUIRE_THROWS_AS(liquidpp::render(templ, c), liquidpp::Exception);
}

TEST_CASE("loop variable same name as range name")
{
   liquidpp::Context c;

   // reduced case found with libFuzzer
   auto templ = R"(
{%- assign handle="cs  ake, cookie, foo" | split: ", " -%}
{% for handle in handle -%}
 {{handle}}
{% endfor %})";
   
   auto rendered = liquidpp::render(templ, c);
   REQUIRE(rendered == R"(cs  ake
cookie
foo
)");
}

TEST_CASE("filter ending on ',' separator")
{
   liquidpp::Context c;

   // reduced case found with libFuzzer
   auto templ = R"(
{%- assign handle="cs  ake, cookie, foo" | split: ", ", -%}
{% for h in handle -%}
 {{h}}
{% endfor %}
   )";
   
   REQUIRE_THROWS_AS(liquidpp::render(templ, c), liquidpp::Exception);
}
