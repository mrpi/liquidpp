#include "catch.hpp"

#include <liquidpp.hpp>

#include <boost/property_tree/json_parser.hpp>

using namespace liquidpp::literals;

TEST_CASE("capture tag")
{
   liquidpp::Context c;
   auto rendered = liquidpp::render(R"({% capture my_variable %}I am being captured.{% endcapture %}{{ my_variable }})", c);
   REQUIRE(rendered == "I am being captured.");
}

TEST_CASE("capture tag with variable in block")
{
   liquidpp::Context c;
   auto rendered = liquidpp::render(R"({% assign y = 123 -%}
{% capture x %}{{y}}{% endcapture -%}
{% assign y = 1234 -%}
{{x}})", c);

   REQUIRE(rendered == "123");
}
