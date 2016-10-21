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

   auto rendered = liquidpp::render("{% for i in (1..5) -%}\n"
                                    "  {% if i == 4 -%}\n"
                                    "    {% break -%}\n"
                                    "  {% else -%}\n"
                                    "    {{ i }} {% endif -%}\n"
                                    "{% endfor %}", c);
   REQUIRE(rendered == "1 2 3 ");
}

TEST_CASE("Iteration: continue")
{
   liquidpp::Context c;

   auto rendered = liquidpp::render("{% for i in (1..5) -%}\n"
                                     "  {% if i == 4 -%}\n"
                                     "    {% continue -%}\n"
                                     "  {% else -%}\n"
                                     "    {{ i -}}\n"
                                     "  {% endif -%}\n"
                                     "{% endfor %}", c);
   REQUIRE(rendered == "1235");
}

TEST_CASE("Iteration: range")
{
   liquidpp::Context c;

   auto rendered = liquidpp::render(R"({% assign num = 4 -%}
{% for i in (1..num) -%}
  {{ i }} {% endfor %})", c);
   REQUIRE(rendered == "1 2 3 4 ");
}

TEST_CASE("Iteration: limit (for parameter)")
{
   liquidpp::Context c;
   c.set("array", std::vector<int>{1,2,3,4,5,6});


   auto rendered = liquidpp::render("{% for item in array limit:2 -%}\n"
                                    " {{ item }} "
                                    "{% endfor %}", c);
   REQUIRE(rendered == "1 2 ");
}
