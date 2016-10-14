#include "catch.hpp"

#include <liquidpp/AnyRange.hpp>

#include <vector>
#include <set>

TEST_CASE("liquidpp::AnyRange<int>::from()")
{
   liquidpp::AnyRange<int> range;
   
   auto initList = {1, 2, 23, 42, 314};
   
   SECTION("std::vector<int>")
   {
      std::vector<int> v = initList;
      range = liquidpp::AnyRange<int>::from(v);
   }

   SECTION("std::set<int>")
   {
      std::set<int> s = initList;
      range = liquidpp::AnyRange<int>::from(s);
   }
   
   for(auto val : initList)
   {
      REQUIRE(range.hasNext());
      REQUIRE(range.next() == val);
   }
   
   REQUIRE_FALSE(range.hasNext());
}

TEST_CASE("liquidpp::AnyRange<int&>::reference()")
{
   std::initializer_list<int> initList = {1, 2, 23, 42, 314};
   
   std::vector<int> v = initList;
   auto vRange = liquidpp::AnyRange<int&>::reference(v);
   const std::set<int> s = initList;
   auto sRange = liquidpp::AnyRange<const int&>::reference(s);
   
   size_t i = 0;
   auto sItr = s.begin();
   for(auto val : initList)
   {      
      REQUIRE(vRange.hasNext());
      auto&& vVal = vRange.next();
      REQUIRE(vVal == val);
      REQUIRE(&vVal == &v[i]);

      REQUIRE(sRange.hasNext());
      auto&& sVal = sRange.next();
      REQUIRE(sVal == val);
      REQUIRE(&sVal == &*sItr);
      
      ++i;
      ++sItr;
   }
   
   REQUIRE_FALSE(vRange.hasNext());
   REQUIRE_FALSE(sRange.hasNext());
}
