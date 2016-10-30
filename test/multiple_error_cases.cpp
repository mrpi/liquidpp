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
   
   try {
      liquidpp::render(templ, c);
      FAIL("Expected an exception!");
   } catch(liquidpp::Exception& e) {
      REQUIRE(e.position().line == 3);
      REQUIRE(e.position().column == 13);
   }
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

TEST_CASE("very long running for loop")
{
   liquidpp::Context c;

   auto templ = liquidpp::parse(R"({% for h in (0..99999999) -%}{% endfor %})");
   
   try {
      templ(c);
      FAIL("Expected an exception!");
   } catch(liquidpp::Exception& e) {
      REQUIRE(e.position().line == 1);
      REQUIRE(e.position().column == 4);
   }
}
