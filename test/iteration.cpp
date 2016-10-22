#include "catch.hpp"

#include <liquidpp.hpp>

TEST_CASE("Iteration: for")
{
   liquidpp::Context c;
   using Product = std::map<std::string, std::string>;
   using Products = std::vector<Product>;
   using ProductCollection = std::map<std::string, Products>;
   Products products{{{"title", "hat"}}, {{"title", "shirt"}}, {{"title", "pants"}}};
   ProductCollection coll{{"products", products}};
   c.set("collection", coll);

   auto rendered = liquidpp::render("{% for product in collection.products %}{{ product.title }} {% endfor %}", c);
   REQUIRE(rendered == "hat shirt pants ");
}

TEST_CASE("Iteration: break")
{
   liquidpp::Context c;

   SECTION("on range of numbers")
   {
      auto rendered = liquidpp::render("{% for i in (1..5) -%}\n"
                                          "  {% if i == 4 -%}\n"
                                          "    {% break -%}\n"
                                          "  {% else -%}\n"
                                          "    {{ i }} {% endif -%}\n"
                                          "{% endfor %}", c);
      REQUIRE(rendered == "1 2 3 ");
   }

   SECTION("on external array")
   {
      c.set("array", std::vector<int>{1,2,3,4,5});
      auto rendered = liquidpp::render("{% for i in array -%}\n"
                                          "  {% if i == 4 -%}\n"
                                          "    {% break -%}\n"
                                          "  {% else -%}\n"
                                          "    {{ i }} {% endif -%}\n"
                                          "{% endfor %}", c);
      REQUIRE(rendered == "1 2 3 ");
   }
}

TEST_CASE("Iteration: continue")
{
   liquidpp::Context c;

   SECTION("on range of numbers")
   {
      auto rendered = liquidpp::render("{% for i in (1..5) -%}\n"
                                          "  {% if i == 4 -%}\n"
                                          "    {% continue -%}\n"
                                          "  {% else -%}\n"
                                          "    {{ i -}}\n"
                                          "  {% endif -%}\n"
                                          "{% endfor %}", c);
      REQUIRE(rendered == "1235");
   }

   SECTION("on external array")
   {
      c.set("array", std::vector<int>{1,2,3,4,5});
      auto rendered = liquidpp::render("{% for i in array -%}\n"
                                          "  {% if i == 4 -%}\n"
                                          "    {% continue -%}\n"
                                          "  {% else -%}\n"
                                          "    {{ i -}}\n"
                                          "  {% endif -%}\n"
                                          "{% endfor %}", c);
      REQUIRE(rendered == "1235");
   }
}

TEST_CASE("Iteration: range")
{
   liquidpp::Context c;

   auto rendered = liquidpp::render(R"(
      {%- assign num = 4 -%}
      {%- for i in (1..num) -%}
        {{- i | append: " " -}}
      {%- endfor -%})", c);
   REQUIRE(rendered == "1 2 3 4 ");
}

TEST_CASE("Iteration: limit ('for' parameter)")
{
   liquidpp::Context c;
   std::string rendered;

   SECTION("on range of numbers")
   {
      rendered = liquidpp::render("{% for item in (1..6) limit:2 -%}\n"
                                  " {{ item }} "
                                  "{% endfor %}", c);
   }

   SECTION("on external array")
   {
      c.set("array", std::vector<int>{1, 2, 3, 4, 5, 6});
      rendered = liquidpp::render("{% for item in array limit:2 -%}\n"
                                  " {{ item }} "
                                  "{% endfor %}", c);
   }

   REQUIRE(rendered == "1 2 ");
}

TEST_CASE("Iteration: offset ('for' parameter)")
{
   liquidpp::Context c;
   std::string rendered;

   SECTION("on range of numbers")
   {
      rendered = liquidpp::render("{% for item in (1..6) offset:2 -%}\n"
                                  " {{ item }} "
                                  "{% endfor %}", c);
   }

   SECTION("on external array")
   {
      c.set("array", std::vector<int>{1, 2, 3, 4, 5, 6});
      rendered = liquidpp::render("{% for item in array offset:2 -%}\n"
                                  " {{ item }} "
                                  "{% endfor %}", c);
   }

   REQUIRE(rendered == "3 4 5 6 ");
}

TEST_CASE("Iteration: reverse ('for' parameter)")
{
   liquidpp::Context c;
   std::string rendered;

   SECTION("on range of numbers")
   {
      rendered = liquidpp::render("{% for item in (1..6) reversed -%}\n"
                                  "  {{ item }} "
                                  "{% endfor %}", c);
   }

   SECTION("on external array")
   {
      c.set("array", std::vector<int>{1, 2, 3, 4, 5, 6});
      rendered = liquidpp::render("{% for item in array reversed -%}\n"
                                  "  {{ item }} "
                                  "{% endfor %}", c);
   }

   REQUIRE(rendered == "6 5 4 3 2 1 ");
}

TEST_CASE("Iteration: limit and offset combined ('for' parameter)")
{
   liquidpp::Context c;
   std::string rendered;

   SECTION("on range of numbers")
   {
      rendered = liquidpp::render("{% for item in (1..6) limit:2 offset:1 -%}\n"
                                     " {{ item }} "
                                     "{% endfor %}", c);
   }

   SECTION("on external array")
   {
      c.set("array", std::vector<int>{1, 2, 3, 4, 5, 6});
      rendered = liquidpp::render("{% for item in array limit:2 offset:1 -%}\n"
                                     " {{ item }} "
                                     "{% endfor %}", c);
   }

   REQUIRE(rendered == "2 3 ");
}

TEST_CASE("Iteration: else in for loop")
{
   liquidpp::Context c;

   auto templ = liquidpp::parse("{% for item in range %}found{% else %}not found{% endfor %}");

   SECTION("on missing range")
   {
      auto rendered = templ(c);
      REQUIRE(rendered == "not found");
   }

   SECTION("on empty range")
   {
      c.set("range", std::vector<std::string>{});
      auto rendered = templ(c);
      REQUIRE(rendered == "not found");
   }

   SECTION("on single value")
   {
      c.set("range", "value");
      auto rendered = templ(c);
      REQUIRE(rendered == "found");
   }

   SECTION("on filled range")
   {
      c.set("range", std::vector<std::string>{"value"});
      auto rendered = templ(c);
      REQUIRE(rendered == "found");
   }
}
