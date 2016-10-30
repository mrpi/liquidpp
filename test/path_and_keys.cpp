#include "catch.hpp"

#include <liquidpp/Key.hpp>

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
