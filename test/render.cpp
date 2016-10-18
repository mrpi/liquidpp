#include "catch.hpp"

#include <liquidpp.hpp>

#ifdef LIQUIDPP_HAVE_RAPIDJSON
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#endif // LIQUIDPP_HAVE_RAPIDJSON

#include <boost/variant/get.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace liquidpp::literals;

namespace RenderUnitTest
{
  constexpr const char* TestTags = "[render]";

  TEST_CASE("render simple text", TestTags)
  {
     REQUIRE("Hello World!" == liquidpp::parse("Hello World!")(liquidpp::Context{}));
  }

  TEST_CASE("render simple string variable", TestTags)
  {
     liquidpp::Context c;
     c.set("myVar", "test");
     
     REQUIRE("test" == liquidpp::parse("{{myVar}}")(c));
  }

  TEST_CASE("render text and string variable", TestTags)
  {
     liquidpp::Context c;
     c.set("name", "Donald Drumpf");
     
     REQUIRE("Hello Donald Drumpf!" == liquidpp::parse("Hello {{name}}!")(c));
  }

  TEST_CASE("render with inline context (initializer_list constructor)", TestTags)
  {
     auto rendered = liquidpp::parse("{{key1}} {{key2}}")({{"key1", std::string{"val1"}}, {"key2", std::string{"val2"}}});
     REQUIRE("val1 val2" == rendered);
  }

  TEST_CASE("render text and integer variable", TestTags)
  {
     liquidpp::Context c;
     c.set("answer", 42);
     
     REQUIRE("The answer is 42!" == liquidpp::parse("The answer is {{ answer }}!")(c));
  }

  TEST_CASE("render std::map<std::string, std::string>", TestTags)
  {
     liquidpp::Context c;
     std::map<std::string, std::string> m{{"a", "value of a"}, {"b", "value of b"}};
     c.set("map", m);
     
     auto render = [&](auto&& str) { return liquidpp::parse(str)(c); };
     REQUIRE("a: value of a" == render("a: {{map.a}}"));
     REQUIRE("b: value of b" == render("b: {{ map.b }}"));
     REQUIRE("c: " == render("c: {{  map.c  }}"));
  }

  TEST_CASE("render std::vector<std::string>", TestTags)
  {
     liquidpp::Context c;
     std::vector<std::string> vec{"value a", "value b"};
     c.set("vec", vec);
     
     auto render = [&](auto&& str) { return liquidpp::parse(str)(c); };
     REQUIRE("#0: value a" == render("#0: {{vec[0]}}"));
     REQUIRE("#1: value b" == render("#1: {{ vec[1] }}"));
     REQUIRE("#2: " == render("#2: {{  vec[2]  }}"));
  }

  TEST_CASE("render std::vector<int>", TestTags)
  {
     liquidpp::Context c;
     std::vector<int> vec{23, 42};
     c.set("vec", vec);

     auto render = [&](auto&& str) { return liquidpp::parse(str)(c); };
     REQUIRE("#0: 23" == render("#0: {{vec[0]}}"));
     REQUIRE("#1: 42" == render("#1: {{ vec[1] }}"));
     REQUIRE("#2: " == render("#2: {{vec[2]}}"));
  }

  TEST_CASE("render std::map<std::string, int>", TestTags)
  {
     liquidpp::Context c;
     std::map<std::string, int> m{{"a", 23}, {"b", 42}};
     c.setAnonymous(m);
     
     auto render = [&](auto&& str) { return liquidpp::parse(str)(c); };
     REQUIRE("a: 23" == render("a: {{a}}"));
     REQUIRE("b: 42" == render("b: {{ b }}"));
     REQUIRE("c: " == render("c: {{  c  }}"));
  }

  TEST_CASE("render boost::property_tree::ptree", TestTags)
  {
     liquidpp::Context c;
     
     boost::property_tree::ptree tree;
     std::istringstream iss(R"(
         <debug>
            <filename>debug.log</filename>
            <modules>
               <module>Finance</module>
               <module>Admin</module>
               <module>HR</module>
            </modules>
            <level>2</level>
         </debug>)");
     boost::property_tree::read_xml(iss, tree);
     c.setAnonymous(tree);
     
     auto render = [&](auto&& str) { return liquidpp::parse(str)(c); };
     REQUIRE("debugFilename: debug.log" == render("debugFilename: {{debug.filename}}"));
     REQUIRE("debugLevel: 2" == render("debugLevel: {{debug.level}}"));
     // TODO; Array
     //REQUIRE("module[0]: Finance" == render("module[0]: {{debug.modules.module}}"));
  }

#ifdef LIQUIDPP_HAVE_RAPIDJSON
  TEST_CASE("render rapidjson::Document", TestTags)
  {
      rapidjson::Document jsonDoc;
      jsonDoc.Parse(R"(
         {
            "title":"example glossary",
            "GlossDiv":{
               "title":"S",
               "pi": 3.1416,
               "GlossList":{
                  "GlossEntry":{
                     "ID":"SGML",
                     "SortAs":"SGML",
                     "GlossTerm":"Standard Generalized Markup Language",
                     "Acronym":"SGML",
                     "Abbrev":"ISO 8879:1986",
                     "GlossDef":{
                        "para":"A meta-markup language, used to create markup languages such as DocBook.",
                        "GlossSeeAlso":[
                           "GML",
                           "XML"
                        ]
                     },
                     "GlossSee":"markup"
                  }
               }
            }
         } )");

      liquidpp::Context c;
      c.setAnonymous(jsonDoc);
     
      auto render = [&](auto&& str) { return liquidpp::parse(str)(c); };

      REQUIRE(render("{{title}}") == "example glossary"_sv);
      REQUIRE(render("{{GlossDiv.GlossList.GlossEntry.ID}}") == "SGML"_sv);
      REQUIRE(render("{{GlossDiv.GlossList.GlossEntry.GlossDef.GlossSeeAlso[1]}}")
              == "XML"_sv);
      REQUIRE(render("{{GlossDiv.pi}}").substr(0, 4) == "3.14"_sv);
  }
#endif // LIQUIDPP_HAVE_RAPIDJSON
}
