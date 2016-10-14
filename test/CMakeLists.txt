include_directories (${liquidpp_SOURCE_DIR}/src
                     ${liquidpp_SOURCE_DIR}/external
                     ${Boost_INCLUDE_DIRS}
                     )

file(DOWNLOAD https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp ${liquidpp_SOURCE_DIR}/external/catch.hpp)

add_executable (liquidppTest
                main.cpp
                block_unit_test.cpp
                path_and_keys.cpp
                render.cpp
                ranges.cpp)
                

target_link_libraries (liquidppTest
                       liquidpp)
                       
add_test(NAME liquidppTest COMMAND liquidppTest)
