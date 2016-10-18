#include "catch.hpp"

#include <liquidpp/parser.hpp>
#include <liquidpp/Expression.hpp>

using namespace liquidpp::literals;

using Tokens = std::vector<liquidpp::string_view>;

TEST_CASE("split expression tokens")
{
   auto check = [](liquidpp::string_view input, const Tokens& expected)
   {
      auto tokens = liquidpp::Expression::splitTokens(input);
      REQUIRE(tokens == expected);
   };

   check("var", {"var"});
   check(" var", {"var"});
   check("var ", {"var"});
   check(" var == 1 ", {"var", "==", "1"});
   check("var==1", {"var", "==", "1"});
   check("'asdf \" 1234 =<>!'", {"'asdf \" 1234 =<>!'"});
   check("\"asdf ' 1234 =<>!\"", {"\"asdf ' 1234 =<>!\""});
   check("true false != <> 3.14", {"true", "false", "!=", "<>", "3.14"});
   check("range[5].value[2]", {"range[5].value[2]"});

   SECTION("Unterminated quoted string")
   {
      REQUIRE_THROWS(liquidpp::Expression::splitTokens(" \" value "));
      REQUIRE_THROWS(liquidpp::Expression::splitTokens(" ' value "));
   }
}
