#include "catch.hpp"

#include <liquidpp.hpp>

#include <boost/property_tree/json_parser.hpp>

#ifdef LIQUIDPP_HAVE_RAPIDJSON
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#endif // LIQUIDPP_HAVE_RAPIDJSON

using namespace liquidpp::literals;

TEST_CASE("parse for tag")
{
   auto templ = liquidpp::parse("{%for var in range%} {%endfor%}");
   REQUIRE(templ.root.nodeList.size() == 1);
   auto tagPtr = boost::get<std::shared_ptr<const liquidpp::IRenderable>>(templ.root.nodeList[0]);
   REQUIRE(tagPtr);
   auto forTag = std::dynamic_pointer_cast<const liquidpp::For>(tagPtr);
   REQUIRE(forTag);
   REQUIRE(forTag->loopVariable == "var");
   REQUIRE(forTag->rangeVariable == "range");
}

TEST_CASE("for loop on std::vector<int>")
{
   std::vector<int> v{1, 2, 23, 42};
   liquidpp::Context c;
   c.set("range", v);

   auto templ = liquidpp::parse("{% for var in range%}{{var}} {%endfor%}");
   auto rendered = templ(c);
   REQUIRE(rendered == "1 2 23 42 ");
}

#ifdef LIQUIDPP_HAVE_RAPIDJSON
TEST_CASE("for loop on rapidjson::Document")
{
   auto json = R"(
      {
         "a": [8, 6, 2],
         "b": [{"subA": 1}, {"subA": 2, "subB": "foo"}, {"subB": 1}]
      })";

   rapidjson::Document jsonDoc;
   jsonDoc.Parse(json);

   liquidpp::Context c;
   c.setAnonymous(jsonDoc);

   SECTION("value array")
   {
      auto templ = liquidpp::parse("{% for var in a%}{{var}} {%endfor%}");
      auto rendered = templ(c);
      REQUIRE(rendered == "8 6 2 ");
   }

   SECTION("object array")
   {
      auto templ = liquidpp::parse("{% for var in b%}{{var.subA}}({{var.subB}}) {%endfor%}");
      auto rendered = templ(c);
      REQUIRE(rendered == "1() 2(foo) (1) ");
   }
}
#endif // LIQUIDPP_HAVE_RAPIDJSON