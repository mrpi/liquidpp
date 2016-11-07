#include "catch.hpp"

#include <liquidpp.hpp>
//#include <liquidpp/accessors/ProtoBuf.hpp>

#ifdef LIQUIDPP_HAVE_RAPIDJSON
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#endif // LIQUIDPP_HAVE_RAPIDJSON

#include <boost/variant/get.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#ifdef LIQUIDPP_HAVE_PROTOBUF
#include "addressbook.pb.h"
#endif

using namespace liquidpp::literals;

namespace RenderUnitTest {
constexpr const char *TestTags = "[render]";

TEST_CASE("render simple text", TestTags) {
  REQUIRE("Hello World!" ==
          liquidpp::parse("Hello World!")(liquidpp::Context{}));
}

TEST_CASE("render simple string variable", TestTags) {
  liquidpp::Context c;
  c.set("myVar", "test");

  REQUIRE("test" == liquidpp::parse("{{myVar}}")(c));
}

TEST_CASE("render text and string variable", TestTags) {
  liquidpp::Context c;
  c.set("name", "Donald Drumpf");

  REQUIRE("Hello Donald Drumpf!" == liquidpp::parse("Hello {{name}}!")(c));
}

TEST_CASE("render with inline context (initializer_list constructor)",
          TestTags) {
  auto rendered = liquidpp::parse("{{key1}} {{key2}}")(
      {{"key1", std::string{"val1"}}, {"key2", std::string{"val2"}}});
  REQUIRE("val1 val2" == rendered);
}

TEST_CASE("render text and integer variable", TestTags) {
  liquidpp::Context c;
  c.set("answer", 42);

  REQUIRE("The answer is 42!" ==
          liquidpp::parse("The answer is {{ answer }}!")(c));
}

TEST_CASE("render std::map<std::string, std::string>", TestTags) {
  liquidpp::Context c;
  std::map<std::string, std::string> m{{"a", "value of a"},
                                       {"b", "value of b"}};
  c.set("map", m);

  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };
  REQUIRE("a: value of a" == render("a: {{map.a}}"));
  REQUIRE("b: value of b" == render("b: {{ map.b }}"));
  REQUIRE("c: " == render("c: {{  map.c  }}"));
}

TEST_CASE("render std::vector<std::string>", TestTags) {
  liquidpp::Context c;
  std::vector<std::string> vec{"value a", "value b"};
  c.set("vec", vec);

  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };
  REQUIRE("#0: value a" == render("#0: {{vec[0]}}"));
  REQUIRE("#1: value b" == render("#1: {{ vec[1] }}"));
  REQUIRE("#2: " == render("#2: {{  vec[2]  }}"));
}

TEST_CASE("render std::vector<int>", TestTags) {
  liquidpp::Context c;
  std::vector<int> vec{23, 42};
  c.set("vec", vec);

  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };
  REQUIRE("#0: 23" == render("#0: {{vec[0]}}"));
  REQUIRE("#1: 42" == render("#1: {{ vec[1] }}"));
  REQUIRE("#2: " == render("#2: {{vec[2]}}"));
}

TEST_CASE("render std::map<std::string, int>", TestTags) {
  liquidpp::Context c;
  std::map<std::string, int> m{{"a", 23}, {"b", 42}};
  c.setAnonymous(m);

  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };
  REQUIRE("a: 23" == render("a: {{a}}"));
  REQUIRE("b: 42" == render("b: {{ b }}"));
  REQUIRE("c: " == render("c: {{  c  }}"));
}

TEST_CASE("render boost::property_tree::ptree", TestTags) {
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

  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };
  REQUIRE("debugFilename: debug.log" ==
          render("debugFilename: {{debug.filename}}"));
  REQUIRE("debugLevel: 2" == render("debugLevel: {{debug.level}}"));
  // TODO; Array
  // REQUIRE("module[0]: Finance" == render("module[0]:
  // {{debug.modules.module}}"));
}

#ifdef LIQUIDPP_HAVE_RAPIDJSON
TEST_CASE("render rapidjson::Document", TestTags) {
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
  c.setAnonymous(std::ref(jsonDoc));

  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };

  REQUIRE(render("{{title}}") == "example glossary"_sv);
  REQUIRE(render("{{GlossDiv.GlossList.GlossEntry.ID}}") == "SGML"_sv);
  REQUIRE(
      render("{{GlossDiv.GlossList.GlossEntry.GlossDef.GlossSeeAlso[1]}}") ==
      "XML"_sv);
  REQUIRE(render("{{GlossDiv.pi}}").substr(0, 4) == "3.14"_sv);
}
#endif // LIQUIDPP_HAVE_RAPIDJSON

TEST_CASE("render std::shared_ptr") {
  liquidpp::Context c;
  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };

  c.set("shared_ptr", std::make_shared<std::string>("Hello"));
  REQUIRE(render("{{shared_ptr}}") == "Hello");

  auto v = std::make_shared<std::vector<int>>();
  *v = {1, 23, 42};
  c.set("shared_ptr", v);
  REQUIRE(render("{{shared_ptr | join: ', '}}") == "1, 23, 42");

  c.set("shared_ptr", std::shared_ptr<std::string>{});
  REQUIRE(render("{{shared_ptr}}") == "");
}

TEST_CASE("render boost::optional") {
  liquidpp::Context c;
  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };

  c.set("optional", boost::optional<std::string>{"Hello"});
  REQUIRE(render("{{optional}}") == "Hello");

  boost::optional<std::vector<int>> v = std::vector<int>{1, 23, 42};
  c.set("optional", v);
  REQUIRE(render("{{optional | join: ', '}}") == "1, 23, 42");

  c.set("optional", boost::optional<std::string>{});
  REQUIRE(render("{{optional}}") == "");
}

TEST_CASE("render std::weak_ptr") {
  liquidpp::Context c;
  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };

  auto shrPtr = std::make_shared<std::string>("Hello");
  std::weak_ptr<std::string> wp = shrPtr;
  c.set("make_shared", wp);
  REQUIRE(render("{{make_shared}}") == "Hello");

  shrPtr.reset();
  REQUIRE(render("{{make_shared}}") == "");
}

TEST_CASE("render std::unique_ptr") {
  liquidpp::Context c;
  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };

  auto uniqPtr = std::make_unique<std::string>("Hello");
  c.set("unique_ptr", std::ref(uniqPtr));
  REQUIRE(render("{{unique_ptr}}") == "Hello");

  uniqPtr.reset();
  REQUIRE(render("{{unique_ptr}}") == "");
}

TEST_CASE("render raw pointer") {
  liquidpp::Context c;
  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };

  std::string val = "Hello";
  auto ptr = &val;
  c.set("ptr", ptr);
  REQUIRE(render("{{ptr}}") == "Hello");

  ptr = nullptr;
  c.set("ptr", ptr);
  REQUIRE(render("{{ptr}}") == "");
}

TEST_CASE("render std::pair") {
  liquidpp::Context c;
  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };

  auto p = std::make_pair("Hello", 42);
  c.set("pair", p);

  REQUIRE(render("{{pair.first}}") == "Hello");
  REQUIRE(render("{{pair.second}}") == "42");
  REQUIRE(render("{{pair.third}}") == "");

  REQUIRE(render("{{pair[0]}}") == "Hello");
  REQUIRE(render("{{pair[1]}}") == "42");
  REQUIRE(render("{{pair[3]}}") == "");
}

TEST_CASE("render std::tuple") {
  liquidpp::Context c;
  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };

  std::vector<int> v{1, 23, 42};
  auto t = std::make_tuple("Hello", 42, 3.14, v);
  c.set("tuple", t);

  REQUIRE(render("{{tuple[0]}}") == "Hello");
  REQUIRE(render("{{tuple[1]}}") == "42");
  REQUIRE(render("{{tuple[2]}}") == "3.14");
  REQUIRE(render("{{tuple[3] | join: ', '}}") == "1, 23, 42");
  REQUIRE(render("{{tuple[4]}}") == "");
}

TEST_CASE("render boost::variant") {
  liquidpp::Context c;
  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };

  boost::variant<std::string, int, double, std::vector<int>> var;
  c.set("variant", std::ref(var));

  var = "Hello";
  REQUIRE(render("{{variant}}") == "Hello");

  var = 42;
  REQUIRE(render("{{variant}}") == "42");

  var = 3.14;
  REQUIRE(render("{{variant}}") == "3.14");

  var = std::vector<int>{1, 23, 42};
  REQUIRE(render("{{variant | join: ', '}}") == "1, 23, 42");
}

#ifdef LIQUIDPP_HAVE_PROTOBUF
TEST_CASE("render Google Protocol Buffers Message") {
  liquidpp::Context c;
  auto render = [&](auto &&str) { return liquidpp::parse(str)(c); };

  tutorial::AddressBook msg;
  auto pers1 = msg.add_people();
  pers1->set_name("Ludger Sprenker");
  pers1->set_id(314);
  // pers1->set_email("");

  auto pers2 = msg.add_people();
  pers2->set_name("Arthur Dent");
  pers2->set_id(42);
  pers2->set_email("hitchhiker@galaxy.uk");
  auto phone = pers2->add_phones();
  phone->set_type(tutorial::Person::MOBILE);
  phone->set_number("555-123456789");

  c.set("msg", msg);
  
  REQUIRE(render("{{msg.people[0].id}}") == "314");
  REQUIRE(render("{{msg.people[0].name}}") == "Ludger Sprenker");
  REQUIRE(render("{{msg.people[0].email}}") == "");

  REQUIRE(render("{{msg.people | map: 'name' | join: ', '}}") ==
          "Ludger Sprenker, Arthur Dent");

  REQUIRE(render("{{msg.people[1].phones[0].number}}") == "555-123456789");
  REQUIRE(render("{{msg.people[1].phones[0].type}}") == "MOBILE");
}
#endif

}
