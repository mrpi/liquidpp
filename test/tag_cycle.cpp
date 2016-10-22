#include "catch.hpp"

#include <liquidpp.hpp>

TEST_CASE("tag cycle")
{
   liquidpp::Context c;

   SECTION("Single simple cycle")
   {
      auto rendered = liquidpp::render(R"({% cycle 'one', 'two', 'three' %}
{% cycle 'one', 'two', 'three' %}
{% cycle 'one', 'two', 'three' %}
{% cycle 'one', 'two', 'three' %})", c);
      auto expected = R"(one
two
three
one)";
      REQUIRE(rendered == expected);
   }

   SECTION("Two cycles interwoven")
   {
      auto rendered = liquidpp::render(R"({% cycle 'one', 'two', 'three' %}
{% cycle 'a', 'b' %}
{% cycle 'one', 'two', 'three' %}
{% cycle 'a', 'b' %}
{% cycle 'one', 'two', 'three' %}
{% cycle 'a', 'b' %}
{% cycle 'one', 'two', 'three' %}
{% cycle 'a', 'b' %})", c);
      auto expected = R"(one
a
two
b
three
a
one
b)";
      REQUIRE(rendered == expected);
   }

   SECTION("Two identical cycle groups")
   {
      auto rendered = liquidpp::render(R"({% cycle 'A': 'one', 'two', 'three' %}
{% cycle 'B': 'one', 'two', 'three' %}
{% cycle 'A': 'one', 'two', 'three' %}
{% cycle 'B': 'one', 'two', 'three' %}
{% cycle 'A': 'one', 'two', 'three' %}
{% cycle 'B': 'one', 'two', 'three' %}
{% cycle 'A': 'one', 'two', 'three' %}
{% cycle 'B': 'one', 'two', 'three' %})", c);
      auto expected = R"(one
one
two
two
three
three
one
one)";
      REQUIRE(rendered == expected);
   }
}
