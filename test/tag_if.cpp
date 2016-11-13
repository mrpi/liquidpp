#include "catch.hpp"

#include <liquidpp.hpp>

#include <liquidpp/tags/Conditional.hpp>

using namespace liquidpp::literals;

TEST_CASE("parse if tag")
{
   liquidpp::Template templ;

   SECTION("unary expression")
   {
      templ = liquidpp::parse("{% if var %} {%endif%}");
      REQUIRE(templ.root.nodeList.size() == 1);
      auto& tagPtr = boost::get<std::unique_ptr<const liquidpp::IRenderable>>(templ.root.nodeList[0]);
      REQUIRE(tagPtr);

      auto ifTag = dynamic_cast<const liquidpp::If*>(tagPtr.get());
      REQUIRE(ifTag);
      REQUIRE(ifTag->expression.tokens.size() == 1);
      //REQUIRE((ifTag->expression.tokens[0] == liquidpp::Expression::VariableName{"var"}));
   }

   SECTION("comparsion expression")
   {
      auto templ = liquidpp::parse("{%if var == '123'%} {%endif%}");
      REQUIRE(templ.root.nodeList.size() == 1);
      auto& tagPtr = boost::get<std::unique_ptr<const liquidpp::IRenderable>>(templ.root.nodeList[0]);
      REQUIRE(tagPtr);
      auto ifTag = dynamic_cast<const liquidpp::If*>(tagPtr.get());
      REQUIRE(ifTag);
      REQUIRE(ifTag->expression.tokens.size() == 3);
   }
}

TEST_CASE("render if unary operator")
{
   std::vector<int> v{1, 2, 23, 42};
   liquidpp::Context c;
   c.set("var", "x");
   c.set("range", v);

   SECTION("existing value")
   {
      auto rendered = liquidpp::render("{% if var %}var is not null{%endif%}", c);
      REQUIRE(rendered == "var is not null");
   }

   SECTION("existing std::vector<int> entry")
   {
      auto rendered = liquidpp::render("{% if range[1] %}range[1] is not null{%endif%}", c);
      REQUIRE(rendered == "range[1] is not null");
   }

   SECTION("out of range on std::vector<int>")
   {
      auto rendered = liquidpp::render("{% if v[4] %}v[4] should be out of range{%endif%}", c);
      REQUIRE(rendered == "");
   }
}

TEST_CASE("render if with comparsion operator")
{
   std::vector<int> v{1, 2, 23, 42};
   liquidpp::Context c;
   c.set("var", "x");
   c.set("range", v);

   SECTION("== on value")
   {
      auto rendered = liquidpp::render("{% if var == 'x' %}matching{%endif%}", c);
      REQUIRE(rendered == "matching");
   }

   SECTION("== on vector entry")
   {
      auto rendered = liquidpp::render("{% if range[3] == 42 %}matching{%endif%}", c);
      REQUIRE(rendered == "matching");
   }

   SECTION("out of range on std::vector<int>")
   {
      auto rendered = liquidpp::render("{% if v[4] == 314 %}matching{%endif%}", c);
      REQUIRE(rendered == "");
   }
}

TEST_CASE("render if logic operator")
{
   std::vector<int> v{1, 2, 23, 42};
   liquidpp::Context c;
   c.set("var", "x");
   c.set("range", v);

   SECTION("and without comarsion")
   {
      auto rendered = liquidpp::render("{% if var and range %}matching{%endif%}", c);
      REQUIRE(rendered == "matching");
   }

   SECTION("and without comarsion - first nil")
   {
      auto rendered = liquidpp::render("{% if var1 and var %}matching{%endif%}", c);
      REQUIRE(rendered == "");
   }

   SECTION("and without comarsion - second nil")
   {
      auto rendered = liquidpp::render("{% if var and var1 %}matching{%endif%}", c);
      REQUIRE(rendered == "");
   }

   SECTION("and with comarsion")
   {
      auto rendered = liquidpp::render("{% if var == 'x' and range[0] == 1 %}matching{%endif%}", c);
      REQUIRE(rendered == "matching");
   }

   SECTION("and with comarsion - second not matching")
   {
      auto rendered = liquidpp::render("{% if var == 'x' and range[0] == 314 %}matching{%endif%}", c);
      REQUIRE(rendered == "");
   }

   SECTION("and with comarsion - first not matching")
   {
      auto rendered = liquidpp::render("{% if range[0] == 314 and var == 'x' %}matching{%endif%}", c);
      REQUIRE(rendered == "");
   }
}
