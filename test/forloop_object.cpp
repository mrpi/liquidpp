// Check examples from https://help.shopify.com/themes/liquid/objects/for-loops

#include "catch.hpp"

#include <liquidpp.hpp>

TEST_CASE("forloop.first")
{
   liquidpp::Context c;
   c.set("products", std::vector<std::string>{{"a", "b", "c", "d", "e"}});

   auto rendered = liquidpp::render(R"(
{% for product in products %}
    {%- if forloop.first == true -%}
        First time through!
    {%- else -%}
        Not the first time.
    {%- endif %}
{% endfor %})", c);
   auto expected = R"(
First time through!
Not the first time.
Not the first time.
Not the first time.
Not the first time.
)";
   REQUIRE(rendered == expected);
}

TEST_CASE("forloop.index")
{
   liquidpp::Context c;
   c.set("products", std::vector<std::string>(16, "a"));

   auto rendered = liquidpp::render(R"(
{%- for product in products -%}
    {{ forloop.index }} {% else %}
    // no products in your frontpage collection
{% endfor %})", c);
   auto expected = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 ";
   REQUIRE(rendered == expected);
}

TEST_CASE("forloop.index0")
{
   liquidpp::Context c;
   c.set("products", std::vector<std::string>(16, "a"));

   auto rendered = liquidpp::render(R"(
{%- for product in products -%}
    {{ forloop.index0 }} {% else %}
    // no products in your frontpage collection
{% endfor %})", c);
   auto expected = "0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 ";
   REQUIRE(rendered == expected);
}

TEST_CASE("forloop.last")
{
   liquidpp::Context c;
   c.set("products", std::vector<std::string>{{"a", "b", "c", "d", "e", "f"}});

   auto rendered = liquidpp::render(R"(
{% for product in products %}
    {%- if forloop.last == true -%}
        This is the last iteration!
    {%- else -%}
        Keep going...
    {%- endif %}
{% endfor %})", c);
   auto expected = R"(
Keep going...
Keep going...
Keep going...
Keep going...
Keep going...
This is the last iteration!
)";
   REQUIRE(rendered == expected);
}

TEST_CASE("forloop.length")
{
   liquidpp::Context c;
   c.set("products", std::vector<std::string>{{"Apple", "Orange", "Peach", "Plum"}});

   auto rendered = liquidpp::render(R"(
{%- for product in products %}
  {%- if forloop.first -%}
    This collection has {{ forloop.length }} products:
{% endif -%}
    {{ product }}
{% endfor %})", c);
   auto expected = R"(This collection has 4 products:
Apple
Orange
Peach
Plum
)";
   REQUIRE(rendered == expected);
}

TEST_CASE("forloop.rindex")
{
   liquidpp::Context c;
   c.set("products", std::vector<std::string>(16, "a"));

   auto rendered = liquidpp::render("{% for product in products %}{{ forloop.rindex }} {% endfor %}", c);
   auto expected = "16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 ";
   REQUIRE(rendered == expected);
}

TEST_CASE("forloop.rindex0")
{
   liquidpp::Context c;
   c.set("products", std::vector<std::string>(16, "a"));

   auto rendered = liquidpp::render("{% for product in products %}{{ forloop.rindex0 }} {% endfor %}", c);
   auto expected = "15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0 ";
   REQUIRE(rendered == expected);
}
