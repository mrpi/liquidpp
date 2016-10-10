#include "catch.hpp"

#include <liquidpp/parser.hpp>

#include <boost/variant/get.hpp>

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
}
