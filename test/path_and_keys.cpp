#include "catch.hpp"

#include <liquidpp/Key.hpp>
#include <liquidpp/Expression.hpp>

using namespace liquidpp::literals;

TEST_CASE("popKey with array index") {
  liquidpp::string_view path = "vec[0]";
  auto key = liquidpp::popKey(path);
  REQUIRE(key);
  REQUIRE(key == "vec"_sv);

  auto key2 = liquidpp::popKey(path);
  REQUIRE(key2.isIndex());
  REQUIRE(key2.index() == 0);

  REQUIRE(path == "");
  REQUIRE_FALSE(liquidpp::popKey(path));
}

TEST_CASE("popKey with variable as array index") {
  liquidpp::string_view path = "vec[i]";
  auto key = liquidpp::popKey(path);
  REQUIRE(key);
  REQUIRE(key == "vec"_sv);

  auto key2 = liquidpp::popKey(path);
  REQUIRE(key2.isIndexVariable());
  REQUIRE(key2.indexVariable().name == "i"_sv);

  REQUIRE(path == "");
  REQUIRE_FALSE(liquidpp::popKey(path));
}

TEST_CASE("popKey with long path") {
  liquidpp::string_view path = "asdftest[123].a[1].x";
  auto key = liquidpp::popKey(path);
  REQUIRE(key);
  REQUIRE(key == "asdftest"_sv);

  auto key2 = liquidpp::popKey(path);
  REQUIRE(key2);
  REQUIRE(key2.index() == 123);

  REQUIRE(path == "a[1].x");

  auto key3 = liquidpp::popKey(path);
  REQUIRE(key3);
  REQUIRE(key3 == "a"_sv);

  auto key4 = liquidpp::popKey(path);
  REQUIRE(key4.isIndex());
  REQUIRE(key4.index() == 1);

  REQUIRE(path == "x");
  auto key5 = liquidpp::popKey(path);
  REQUIRE(key5);
  REQUIRE(key5 == "x"_sv);
  REQUIRE_FALSE(key.isIndex());

  REQUIRE(path == "");
  REQUIRE_FALSE(liquidpp::popKey(path));
}

TEST_CASE("popKey loop") {
  liquidpp::string_view path = "asdftest[123].a[1].x";
  while (auto key = liquidpp::popKey(path)) {
    REQUIRE(key);
    if (key.isName())
      REQUIRE_FALSE(key.name().empty());
    else
      REQUIRE(key.isIndex());
  }

  REQUIRE(path.empty());
}

TEST_CASE("hash key with whitespace") {
  liquidpp::string_view path = "asdftest['Hello world!='].a[1].x";
  auto tokens = liquidpp::Expression::splitTokens(path);
  REQUIRE(tokens.size() == 1);
  REQUIRE(tokens[0] == path);
  
  auto pathObject = liquidpp::toPath(tokens[0]);
  REQUIRE(pathObject.size() == 5);
  REQUIRE(pathObject[0].name() == "asdftest");
  REQUIRE(pathObject[1].name() == "Hello world!=");
  REQUIRE(pathObject[2].name() == "a");
  REQUIRE(pathObject[3].index() == 1);
  REQUIRE(pathObject[4].name() == "x");
}

TEST_CASE("multi dimensional array") {
   SECTION("String index after number index")
   {
      liquidpp::string_view path = "asdftest[42][\"He[ll]o!\"]";
      auto tokens = liquidpp::Expression::splitTokens(path);
      REQUIRE(tokens.size() == 1);

      auto pathObject = liquidpp::toPath(tokens[0]);
      REQUIRE(pathObject.size() == 3);
      REQUIRE(pathObject[0].name() == "asdftest");
      REQUIRE(pathObject[1].index() == 42);
      REQUIRE(pathObject[2].name() == "He[ll]o!");
   }

   SECTION("Number index after string index")
   {
      liquidpp::string_view path = "asdftest[\"He[ll]o!\"][42]";
      auto tokens = liquidpp::Expression::splitTokens(path);
      REQUIRE(tokens.size() == 1);

      auto pathObject = liquidpp::toPath(tokens[0]);
      REQUIRE(pathObject.size() == 3);
      REQUIRE(pathObject[0].name() == "asdftest");
      REQUIRE(pathObject[1].name() == "He[ll]o!");
      REQUIRE(pathObject[2].index() == 42);
   }
}
