#include <catch.hpp>

#include <liquidpp/parser.hpp>

#include <boost/variant/get.hpp>

namespace BlockUnitTest
{
  constexpr const char* TestTags = "[block]";
  
  using NodeList = std::vector<liquidpp::Node>;
  using liquidpp::NodeType;

  constexpr auto NodeTypeConditional = NodeType::Tag;

  std::vector<NodeType> blockTypes(const NodeList& l)
  {
     std::vector<NodeType> res;
     res.reserve(l.size());
     for(auto&& n : l)
        res.push_back(liquidpp::type(n));
     return res;
  }
  
  TEST_CASE("blankspace", TestTags)
  {
    auto template_ = liquidpp::parse("  ");
    REQUIRE((template_.root.nodeList == NodeList{"  "}));
  }

  TEST_CASE("variable_beginning", TestTags)
  {
    auto template_ = liquidpp::parse("{{funk}}  ");
    REQUIRE(2 == template_.root.nodeList.size());
    REQUIRE(NodeType::Variable == liquidpp::type(template_.root.nodeList[0]));
    REQUIRE(NodeType::String == liquidpp::type(template_.root.nodeList[1]));
  }

  TEST_CASE("variable_end", TestTags)
  {
    auto template_ = liquidpp::parse("  {{funk}}");
    REQUIRE(2 == template_.root.nodeList.size());
    REQUIRE(NodeType::String == liquidpp::type(template_.root.nodeList[0]));
    REQUIRE(NodeType::Variable == liquidpp::type(template_.root.nodeList[1]));
  }

  TEST_CASE("variable middle", TestTags)
  {
    for (auto str : {"  {{funk}}  ", "  {{ funk}}  ", "  {{funk }}  ", "  {{ funk }}  ", "  {{  funk  }}  "})
    {
      SECTION(str) 
      {
         auto template_ = liquidpp::parse(str);
         REQUIRE(3 == template_.root.nodeList.size());
         REQUIRE(NodeType::String == liquidpp::type(template_.root.nodeList[0]));
         REQUIRE(NodeType::Variable == liquidpp::type(template_.root.nodeList[1]));
         REQUIRE(NodeType::String == liquidpp::type(template_.root.nodeList[2]));
      }
    }
  }

  TEST_CASE("variable many embedded fragments", TestTags)
  {
    auto template_ = liquidpp::parse("  {{funk}} {{so}} {{brother}} ");
    REQUIRE(7 == template_.root.nodeList.size());
    std::vector<NodeType> expected{NodeType::String, NodeType::Variable, NodeType::String, NodeType::Variable, NodeType::String, NodeType::Variable, NodeType::String};
    REQUIRE(expected == blockTypes(template_.root.nodeList));
  }

  TEST_CASE("with block", TestTags)
  {
    for (auto str : {"  {%comment%} {%endcomment%} ", "  {% comment%} {%endcomment %} ", "  {% comment %} {% endcomment %} ", "  {%  comment  %} {%  endcomment  %} "})
    {
      SECTION(str) 
      {
         auto template_ = liquidpp::parse(str);
         std::vector<NodeType> expected{NodeType::String, NodeType::Tag, NodeType::String};
         REQUIRE(expected == blockTypes(template_.root.nodeList));
         REQUIRE(3 == template_.root.nodeList.size());
      }
    }
  }

  TEST_CASE("conditional block", TestTags)
  {
      auto template_ = liquidpp::parse(R"(
{% if product.title == 'Awesome Shoes' %}
  These shoes are awesome!
{% endif %}         
      )");
      std::vector<NodeType> expected{NodeType::String, NodeTypeConditional, NodeType::String};
      REQUIRE(expected == blockTypes(template_.root.nodeList));
      REQUIRE(3 == template_.root.nodeList.size());
  }

#if 0
  TEST_CASE("with custom tag", TestTags)
  {
    auto template_ = liquidpp::parse("{% testtag %} {% endtesttag %}");
    REQUIRE(1 == template_.root.nodeList.size());
    REQUIRE(NodeType::UnevaluatedTag == liquidpp::type(template_.root.nodeList[0]));
    REQUIRE("testtag" == boost::get<liquidpp::UnevaluatedTag>(template_.root.nodeList[0]).name);
  }
#endif
}
