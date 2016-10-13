#include "catch.hpp"

#include <liquidpp/Key.hpp>

using namespace liquidpp::literals;

TEST_CASE("popKey with array index")
{
   liquidpp::string_view path = "vec[0]";
   auto key = liquidpp::popKey(path);
   REQUIRE(key);
   REQUIRE(key.name == "vec"_sv);
   REQUIRE(key.idx);
   REQUIRE(*key.idx == 0);
   REQUIRE(path == "");

   REQUIRE_FALSE(liquidpp::popKey(path));
}

TEST_CASE("popKey with long path")
{
   liquidpp::string_view path = "asdftest[123].a[1].x";
   auto key = liquidpp::popKey(path);
   REQUIRE(key);
   REQUIRE(key.name == "asdftest"_sv);
   REQUIRE(key.idx);
   REQUIRE(*key.idx == 123);
   REQUIRE(path == "a[1].x");

   key = liquidpp::popKey(path);
   REQUIRE(key);
   REQUIRE(key.name == "a"_sv);
   REQUIRE(key.idx);
   REQUIRE(*key.idx == 1);
   REQUIRE(path == "x");

   key = liquidpp::popKey(path);
   REQUIRE(key);
   REQUIRE(key.name == "x"_sv);
   REQUIRE_FALSE(key.idx);
   REQUIRE(path == "");

   REQUIRE_FALSE(liquidpp::popKey(path));
}

TEST_CASE("popKey loop")
{
   liquidpp::string_view path = "asdftest[123].a[1].x";
   while (auto key = liquidpp::popKey(path))
   {
      REQUIRE(key);
      REQUIRE_FALSE(key.name.empty());
   }

   REQUIRE(path.empty());
}

