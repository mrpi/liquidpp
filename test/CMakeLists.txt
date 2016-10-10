include_directories (${liquidpp_SOURCE_DIR}/src
                     ${Boost_INCLUDE_DIRS}
                     )

add_executable (liquidppTest
                main.cpp
                block_unit_test.cpp
                render.cpp)
                

target_link_libraries (liquidppTest
                       liquidpp)
                       
add_test(NAME liquidppTest COMMAND liquidppTest)
