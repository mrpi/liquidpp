#include "catch.hpp"

#include <liquidpp.hpp>

TEST_CASE("Control flow: case/when")
{
   liquidpp::Context c;
   auto templ = liquidpp::parse(R"({%- case handle -%}
  {%- when 'cake' -%}
     This is a cake
  {%- when 'cookie' -%}
     This is a cookie
  {%- else -%}
     This is not a cake nor a cookie
{%- endcase -%})");

   c.set("handle", "cake");
   REQUIRE(templ(c) == "This is a cake");

   c.set("handle", "cookie");
   REQUIRE(templ(c) == "This is a cookie");

   c.set("handle", "donut");
   REQUIRE(templ(c) == "This is not a cake nor a cookie");
}

TEST_CASE("Control flow: if")
{
   liquidpp::Context c;
   auto templ = liquidpp::parse(R"({%- if product.title == 'Awesome Shoes' -%}
  These shoes are awesome!
{%- endif -%})");

   SECTION("Matching")
   {
      c.set("product", std::map<std::string, std::string>{{"title", "Awesome Shoes"}});
      REQUIRE(templ(c) == "These shoes are awesome!");
   }

   SECTION("Not matching")
   {
      c.set("product", std::map<std::string, std::string>{{"title", "Lame Shoes"}});
      REQUIRE(templ(c) == "");
   }
}

TEST_CASE("Control flow: unless")
{
   liquidpp::Context c;
   auto templ = liquidpp::parse(R"({%- unless product.title == 'Awesome Shoes' -%}
  These shoes are not awesome.
{%- endunless -%})");

   SECTION("Matching")
   {
      c.set("product", std::map<std::string, std::string>{{"title", "Awesome Shoes"}});
      REQUIRE(templ(c) == "");
   }

   SECTION("Not matching")
   {
      c.set("product", std::map<std::string, std::string>{{"title", "Lame Shoes"}});
      REQUIRE(templ(c) == "These shoes are not awesome.");
   }
}

TEST_CASE("Control flow: elsif / else")
{
   liquidpp::Context c;
   auto templ = liquidpp::parse(R"({%- if customer.name == 'kevin' -%}
  Hey Kevin!
{%- elsif customer.name == 'anonymous' -%}
  Hey Anonymous!
{%- else -%}
  Hi Stranger!
{%- endif -%})");

   SECTION("if matching")
   {
      c.set("customer", std::map<std::string, std::string>{{"name", "kevin"}});
      REQUIRE(templ(c) == "Hey Kevin!");
   }

   SECTION("elsif matching")
   {
      c.set("customer", std::map<std::string, std::string>{{"name", "anonymous"}});
      REQUIRE(templ(c) == "Hey Anonymous!");
   }

   SECTION("none matching")
   {
      c.set("customer", std::map<std::string, std::string>{{"name", "mrpi"}});
      REQUIRE(templ(c) == "Hi Stranger!");
   }
}
