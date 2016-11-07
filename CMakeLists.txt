cmake_minimum_required(VERSION 2.8)
project(liquidpp)

INCLUDE(CheckIncludeFileCXX)

find_package( Boost REQUIRED COMPONENTS locale )
include_directories(${Boost_INCLUDE_DIRS}) 

find_package(Protobuf)
if (PROTOBUF_FOUND)
   set(LIQUIDPP_HAVE_PROTOBUF PROTOBUF_FOUND)
endif (PROTOBUF_FOUND)
    
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

if(MSVC)
   # We just need the boost::date_time heades not the lib
   add_definitions(-DBOOST_ALL_NO_LIB)
else()
   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror")
endif()

check_include_file_cxx(string_view LIQUIDPP_HAVE_STD_STRING_VIEW) 
check_include_file_cxx(experimental/string_view LIQUIDPP_HAVE_STD_EXPERIMENTAL_STRING_VIEW) 
check_include_file_cxx(boost/utility/string_view.hpp LIQUIDPP_HAVE_BOOST_STRING_VIEW) 
check_include_file_cxx(nonius/nonius_single.hpp LIQUIDPP_HAVE_NONIUS)
check_include_file_cxx(rapidjson/rapidjson.h LIQUIDPP_HAVE_RAPIDJSON)
check_include_file_cxx(boost/container/small_vector.hpp LIQUIDPP_HAVE_BOOST_SMALL_VECTOR)

if (LIQUIDPP_HAVE_STD_STRING_VIEW)
   message("using std::string_view")
   set(LIQUIDPP_STRING_VIEW_HEADER <string_view>)
   set(LIQUIDPP_STRING_VIEW_CLASS std::string_view)
elseif (LIQUIDPP_HAVE_STD_EXPERIMENTAL_STRING_VIEW)
   message("using std::experimental::string_view")
   set(LIQUIDPP_STRING_VIEW_HEADER <experimental/string_view>)
   set(LIQUIDPP_STRING_VIEW_CLASS std::experimental::string_view)
elseif (LIQUIDPP_HAVE_BOOST_STRING_VIEW)
   message("using boost::string_view")
   set(LIQUIDPP_STRING_VIEW_HEADER <boost/utility/string_view.hpp>)
   set(LIQUIDPP_STRING_VIEW_CLASS boost::string_view)
else (LIQUIDPP_HAVE_STD_STRING_VIEW)
   message("using boost::string_view (shipped version)")
   set(LIQUIDPP_STRING_VIEW_HEADER \"external/string_view.hpp\")
   set(LIQUIDPP_STRING_VIEW_CLASS boost::string_view)
endif (LIQUIDPP_HAVE_STD_STRING_VIEW)

if (!LIQUIDPP_HAVE_BOOST_SMALL_VECTOR)
   message("boost::container::small_vector is not available (using std::vector instead)")
endif(!LIQUIDPP_HAVE_BOOST_SMALL_VECTOR)

add_subdirectory (src) 

enable_testing()
add_subdirectory (test)

if (LIQUIDPP_HAVE_NONIUS)
  add_subdirectory (benchmark)
else (LIQUIDPP_HAVE_NONIUS)
   message("not building benchmarks (nonius library is missing)")
endif (LIQUIDPP_HAVE_NONIUS)
