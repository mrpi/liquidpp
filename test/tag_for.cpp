#include "catch.hpp"

#include <liquidpp.hpp>

#include <boost/property_tree/json_parser.hpp>

#ifdef LIQUIDPP_HAVE_RAPIDJSON
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <liquidpp/tags/For.hpp>

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

TEST_CASE("for loop on single value")
{
   liquidpp::Context c;
   c.set("range", "Drumpf");

   auto rendered = liquidpp::render("{% for var in range%}{{var}} {%endfor%}", c);
   REQUIRE(rendered == "Drumpf ");
}

TEST_CASE("for loop on std::vector<int>")
{
   std::vector<int> v{1, 2, 23, 42};
   liquidpp::Context c;
   c.set("range", v);

   auto rendered = liquidpp::render("{% for var in range%}{{var}} {%endfor%}", c);
   REQUIRE(rendered == "1 2 23 42 ");
}

constexpr auto json = R"(
      {
         "a": [8, 6, 2],
         "b": [{"subA": 1}, {"subA": 2, "subB": "foo"}, {"subB": 1}],
         "c": "Hello"
      })";

TEST_CASE("for loop on boost::property_tree")
{
   boost::property_tree::ptree pt;
   std::istringstream ss{json};
   boost::property_tree::read_json(ss, pt);

   liquidpp::Context c;
   c.setAnonymous(pt);

   SECTION("value array")
   {
      auto rendered = liquidpp::render("{% for var in a%}{{var}} {%endfor%}", c);
      REQUIRE(rendered == "8 6 2 ");
   }

   SECTION("object array")
   {
      auto rendered = liquidpp::render("{% for var in b%}{{var.subA}}({{var.subB}}) {%endfor%}", c);
      REQUIRE(rendered == "1() 2(foo) (1) ");
   }
}

#ifdef LIQUIDPP_HAVE_RAPIDJSON
TEST_CASE("for loop on rapidjson::Document")
{
   rapidjson::Document jsonDoc;
   jsonDoc.Parse(json);

   liquidpp::Context c;
   c.setAnonymous(jsonDoc);

   SECTION("value array")
   {
      auto rendered = liquidpp::render("{% for var in a%}{{var}} {%endfor%}", c);
      REQUIRE(rendered == "8 6 2 ");
   }

   SECTION("object array")
   {
      auto rendered = liquidpp::render("{% for var in b%}{{var.subA}}({{var.subB}}) {%endfor%}", c);
      REQUIRE(rendered == "1() 2(foo) (1) ");
   }
}
#endif // LIQUIDPP_HAVE_RAPIDJSON