cmake_minimum_required(VERSION 2.8)
project(liquidpp)

INCLUDE(CheckIncludeFileCXX)

find_package( Boost REQUIRED )
include_directories(${Boost_INCLUDE_DIRS}) 
    
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

check_include_file_cxx(string_view LIQUIDPP_HAVE_STD_STRING_VIEW) 
check_include_file_cxx(experimental/string_view LIQUIDPP_HAVE_STD_EXPERIMENTAL_STRING_VIEW) 
check_include_file_cxx(boost/utility/string_view.hpp LIQUIDPP_HAVE_BOOST_STRING_VIEW) 
check_include_file_cxx(nonius/nonius_single.h++ LIQUIDPP_HAVE_NONIUS)

if (LIQUIDPP_HAVE_STD_STRING_VIEW)
   message("using std::string_view")
   set(LIQUIDPP_STRING_VIEW_HEADER <string_view>)
   set(LIQUIDPP_STRING_VIEW_CLASS std::string_view)
#elseif (LIQUIDPP_HAVE_STD_EXPERIMENTAL_STRING_VIEW)
#   message("using std::experimental::string_view")
#   set(LIQUIDPP_STRING_VIEW_HEADER <experimental/string_view>)
#   set(LIQUIDPP_STRING_VIEW_CLASS std::experimental::string_view)
elseif (LIQUIDPP_HAVE_BOOST_STRING_VIEW)
   message("using boost::string_view")
   set(LIQUIDPP_STRING_VIEW_HEADER <boost/utility/string_view.hpp>)
   set(LIQUIDPP_STRING_VIEW_CLASS boost::string_view)
else (LIQUIDPP_HAVE_STD_STRING_VIEW)
   message("using boost::string_view (shipped version)")
   set(LIQUIDPP_STRING_VIEW_HEADER \"external/string_view.hpp\")
   set(LIQUIDPP_STRING_VIEW_CLASS boost::string_view)
endif (LIQUIDPP_HAVE_STD_STRING_VIEW)

add_subdirectory (src) 

enable_testing()
add_subdirectory (test)

if (LIQUIDPP_HAVE_NONIUS)
  add_subdirectory (benchmark)
else (LIQUIDPP_HAVE_NONIUS)
   message("not building benchmarks (nonius library is missing)")
endif (LIQUIDPP_HAVE_NONIUS)
