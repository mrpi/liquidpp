#include "catch.hpp"

#include <liquidpp.hpp>

#include <liquidpp/tags/If.hpp>

using namespace liquidpp::literals;

TEST_CASE("parse if tag")
{
   liquidpp::Template templ;

   SECTION("unary expression")
   {
      templ = liquidpp::parse("{% if var %} {%endif%}");
      REQUIRE(templ.root.nodeList.size() == 1);
      auto tagPtr = boost::get<std::shared_ptr<const liquidpp::IRenderable>>(templ.root.nodeList[0]);
      REQUIRE(tagPtr);

      auto ifTag = std::dynamic_pointer_cast<const liquidpp::If>(tagPtr);
      REQUIRE(ifTag);
      REQUIRE(ifTag->variable == "var");
   }

   SECTION("comparsion expression")
   {
      auto templ = liquidpp::parse("{%if var == '123'%} {%endif%}");
      REQUIRE(templ.root.nodeList.size() == 1);
      auto tagPtr = boost::get<std::shared_ptr<const liquidpp::IRenderable>>(templ.root.nodeList[0]);
      REQUIRE(tagPtr);
      auto ifTag = std::dynamic_pointer_cast<const liquidpp::If>(tagPtr);
      REQUIRE(ifTag);
      REQUIRE(ifTag->variable == "var");
      REQUIRE(ifTag->compareWith.operator_ == "==");
      REQUIRE(ifTag->compareWith.value == "123");
   }
}
